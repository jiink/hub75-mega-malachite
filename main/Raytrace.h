

#pragma once
#include "IApplet.h"
#include "JaDraw.h"
#include "vmath_all.hpp"
#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>

// Define constants for ray tracing (unchanged)
constexpr float EPSILON = 1e-4f;
constexpr int MAX_REFLECTION_DEPTH = 3;

// --- Structs (Ray, Material, HitInfo - unchanged) ---
struct Ray {
    vmath_hpp::fvec3 origin;
    vmath_hpp::fvec3 direction; // Should be normalized
};

struct Material {
    vmath_hpp::fvec3 color;
    float reflectivity = 0.0f;
    float ambient = 0.1f;
    float diffuse = 0.7f;
    float specular = 0.2f;
    float shininess = 32.0f;
};

struct HitInfo {
    bool hit = false;
    float t = std::numeric_limits<float>::max();
    vmath_hpp::fvec3 point;
    vmath_hpp::fvec3 normal;
    const Material* material = nullptr;
};
// --- End Structs ---

// Helper function to convert HSV to RGB (needed for color cycling)
// H: [0, 360), S: [0, 1], V: [0, 1]
vmath_hpp::fvec3 hsvToRgb(float h, float s, float v) {
    if (s <= 0.0f) { return vmath_hpp::fvec3(v, v, v); } // achromatic (grey)

    h /= 60.0f; // sector 0 to 5
    int i = static_cast<int>(std::floor(h));
    float f = h - i; // factorial part of h
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t_col = v * (1.0f - s * (1.0f - f)); // Renamed from 't' to avoid conflict

    switch (i) {
        case 0: return vmath_hpp::fvec3(v, t_col, p);
        case 1: return vmath_hpp::fvec3(q, v, p);
        case 2: return vmath_hpp::fvec3(p, v, t_col);
        case 3: return vmath_hpp::fvec3(p, q, v);
        case 4: return vmath_hpp::fvec3(t_col, p, v);
        default: // case 5:
            return vmath_hpp::fvec3(v, p, q);
    }
}


class Raytrace : public IApplet {
public:
    Raytrace();
    ~Raytrace() override = default;
    void setup() override;
    void loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) override;
    const char* getName() const override;

private:
    // Scene objects
    vmath_hpp::fvec3 sphereBasePos; // Store the base position for animation calculation
    vmath_hpp::fvec3 spherePos;
    float sphereRadius;
    Material sphereMaterial; // Material color will be updated dynamically

    vmath_hpp::fvec3 planeNormal;
    float planeDist;
    Material planeMaterial1;
    Material planeMaterial2;

    // Light
    vmath_hpp::fvec3 lightDir; // Will be updated dynamically
    vmath_hpp::fvec3 lightColor;
    float lightAngle = 0.0f;   // For animating light direction

    // Camera
    vmath_hpp::fvec3 camPos;    // Will be updated dynamically
    vmath_hpp::fvec3 camTarget; // Point the camera looks at
    float camOrbitRadius = 5.0f;
    float camOrbitAngle = 0.0f;
    float camHeight = 2.0f;

    // Timing and Animation state
    float t = 0.0f;
    vmath_hpp::fvec2 gridOffset = vmath_hpp::fvec2(0.0f, 0.0f); // For animating grid

    // Raytracing functions
    bool intersectSphere(const Ray& ray, const vmath_hpp::fvec3& center, float radius, float& t_hit) const; // Renamed t -> t_hit
    bool intersectPlane(const Ray& ray, const vmath_hpp::fvec3& normal, float d, float& t_hit) const; // Renamed t -> t_hit
    HitInfo traceScene(const Ray& ray) const;
    vmath_hpp::fvec3 castRay(const Ray& ray, int depth) const;
    vmath_hpp::fvec3 getPlaneColor(const vmath_hpp::fvec3& point) const; // Now takes gridOffset
};

// --- Implementation ---

Raytrace::Raytrace() { }

void Raytrace::setup() {
    // Initialize scene
    sphereBasePos = vmath_hpp::fvec3(0.0f, 1.0f, 6.0f); // Base for animation
    spherePos = sphereBasePos; // Initial position
    sphereRadius = 1.0f;
    // sphereMaterial.color set dynamically in loop
    sphereMaterial.reflectivity = 0.7f;
    sphereMaterial.ambient = 0.1f;
    sphereMaterial.diffuse = 0.6f;
    sphereMaterial.specular = 0.8f;
    sphereMaterial.shininess = 100.0f;

    planeNormal = vmath_hpp::fvec3(0.0f, 1.0f, 0.0f);
    planeDist = 0.0f;
    planeMaterial1.color = vmath_hpp::fvec3(0.9f, 0.9f, 0.9f);
    planeMaterial1.reflectivity = 0.0f;
    planeMaterial1.ambient = 0.1f;
    planeMaterial1.diffuse = 0.8f;
    planeMaterial1.specular = 0.1f;
    planeMaterial1.shininess = 10.0f;

    planeMaterial2.color = vmath_hpp::fvec3(0.1f, 0.1f, 0.1f);
    planeMaterial2.reflectivity = 0.0f;
    planeMaterial2.ambient = 0.1f;
    planeMaterial2.diffuse = 0.8f;
    planeMaterial2.specular = 0.1f;
    planeMaterial2.shininess = 10.0f;

    // Initial light direction (will be updated)
    lightDir = vmath_hpp::normalize(vmath_hpp::fvec3(-0.5f, -0.8f, -1.0f));
    lightColor = vmath_hpp::fvec3(1.0f, 1.0f, 0.8f);

    // Initial camera setup (will be updated)
    camTarget = vmath_hpp::fvec3(0.0f, 0.5f, 6.0f); // Look towards the general area
    camPos = vmath_hpp::fvec3(0.0f, camHeight, camTarget.z - camOrbitRadius); // Start behind target
}

// fvec3ToRGBA function (unchanged)
uint32_t fvec3ToRGBA(vmath_hpp::fvec3& floatColor) {
    floatColor.x = std::max(0.0f, std::min(1.0f, floatColor.x));
    floatColor.y = std::max(0.0f, std::min(1.0f, floatColor.y));
    floatColor.z = std::max(0.0f, std::min(1.0f, floatColor.z));
    uint8_t r = static_cast<uint8_t>(floatColor.x * 255.0f);
    uint8_t g = static_cast<uint8_t>(floatColor.y * 255.0f);
    uint8_t b = static_cast<uint8_t>(floatColor.z * 255.0f);
    uint8_t a = 255;
    return JADRAW_RGBA(r, g, b, a);
}

// Ray-intersection functions (intersectSphere, intersectPlane - Renamed t -> t_hit)
bool Raytrace::intersectSphere(const Ray& ray, const vmath_hpp::fvec3& center, float radius, float& t_hit) const {
    vmath_hpp::fvec3 oc = ray.origin - center;
    float a = vmath_hpp::dot(ray.direction, ray.direction);
    float b = 2.0f * vmath_hpp::dot(oc, ray.direction);
    float c = vmath_hpp::dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return false;
    float sqrt_discriminant = std::sqrt(discriminant);
    float t1 = (-b - sqrt_discriminant) / (2.0f * a);
    float t2 = (-b + sqrt_discriminant) / (2.0f * a);
    if (t1 > EPSILON && (t1 < t2 || t2 <= EPSILON)) { t_hit = t1; return true; }
    if (t2 > EPSILON) { t_hit = t2; return true; }
    return false;
}

bool Raytrace::intersectPlane(const Ray& ray, const vmath_hpp::fvec3& normal, float d, float& t_hit) const {
    float denom = vmath_hpp::dot(normal, ray.direction);
    if (std::abs(denom) > EPSILON) {
        float dist = d - vmath_hpp::dot(normal, ray.origin);
        float t_candidate = dist / denom;
        if (t_candidate > EPSILON) { t_hit = t_candidate; return true; }
    }
    return false;
}

// *** MODIFIED getPlaneColor function ***
// Now uses the member gridOffset
vmath_hpp::fvec3 Raytrace::getPlaneColor(const vmath_hpp::fvec3& point) const {
    float scale = 0.1f;
    // Apply offset for movement
    float checkCoordX = point.x * scale + gridOffset.x;
    float checkCoordZ = point.z * scale + gridOffset.y; // Use gridOffset.y for Z axis
    // Floor checks
    int checkX = static_cast<int>(std::floor(checkCoordX));
    int checkZ = static_cast<int>(std::floor(checkCoordZ));
    // Ensure negative coordinates work correctly with modulo
    bool isWhite = ((checkX % 2 == 0) ^ (checkZ % 2 == 0)); // Use XOR for checker pattern
    return isWhite ? planeMaterial1.color : planeMaterial2.color;
}


// traceScene function (updated to pass potentially modified sphereMaterial.color)
HitInfo Raytrace::traceScene(const Ray& ray) const {
    HitInfo closestHit;
    closestHit.hit = false;
    closestHit.t = std::numeric_limits<float>::max();
    float t_hit = std::numeric_limits<float>::max();

    // Intersect with sphere
    if (intersectSphere(ray, spherePos, sphereRadius, t_hit)) {
        if (t_hit < closestHit.t) {
            closestHit.hit = true;
            closestHit.t = t_hit;
            closestHit.point = ray.origin + ray.direction * t_hit;
            closestHit.normal = vmath_hpp::normalize(closestHit.point - spherePos);
            closestHit.material = &sphereMaterial; // Pass the sphere material
        }
    }
    // Intersect with plane
    if (intersectPlane(ray, planeNormal, planeDist, t_hit)) {
        if (t_hit < closestHit.t) {
            closestHit.hit = true;
            closestHit.t = t_hit;
            closestHit.point = ray.origin + ray.direction * t_hit;
            closestHit.normal = planeNormal;
            // Determine checkerboard material based on *final* color needed in castRay
            // The material struct passed here just needs the properties (reflectivity etc)
            // The actual color will be determined by getPlaneColor in castRay
            int checkX = static_cast<int>(std::floor(closestHit.point.x + gridOffset.x));
            int checkZ = static_cast<int>(std::floor(closestHit.point.z + gridOffset.y));
             closestHit.material = ((checkX % 2 == 0) ^ (checkZ % 2 == 0)) ? &planeMaterial1 : &planeMaterial2;
        }
    }
    return closestHit;
}

// castRay function (updated to use dynamic sphere/plane colors)
vmath_hpp::fvec3 Raytrace::castRay(const Ray& ray, int depth) const {
    vmath_hpp::fvec3 backgroundColor(0.1f, 0.2f, 0.4f);
    if (depth > MAX_REFLECTION_DEPTH) return backgroundColor;

    HitInfo hitInfo = traceScene(ray);
    if (hitInfo.hit) {
        const Material& mat = *hitInfo.material; // Get material properties (reflectivity etc.)

        // *** Determine surface color dynamically ***
        vmath_hpp::fvec3 surfaceColor;
        if (hitInfo.material == &sphereMaterial) {
            surfaceColor = mat.color; // Sphere color is already updated in loop
        } else {
            surfaceColor = getPlaneColor(hitInfo.point); // Get checkerboard color based on hit point + offset
        }
        // *** --- ***

        vmath_hpp::fvec3 hitPoint = hitInfo.point;
        vmath_hpp::fvec3 normal = hitInfo.normal;
        // *** Use dynamic camera position for view direction ***
        vmath_hpp::fvec3 viewDir = vmath_hpp::normalize(camPos - hitPoint);
        // *** --- ***

        vmath_hpp::fvec3 finalColor = surfaceColor * mat.ambient; // Start with ambient

        Ray shadowRay;
        shadowRay.origin = hitPoint + normal * EPSILON;
        // *** Use dynamic light direction for shadow ray ***
        shadowRay.direction = -lightDir;
        // *** --- ***
        HitInfo shadowHit = traceScene(shadowRay);
        bool inShadow = shadowHit.hit;

        if (!inShadow) {
            // Diffuse
            // *** Use dynamic light direction for diffuse calculation ***
            float diff = std::max(0.0f, vmath_hpp::dot(normal, -lightDir));
            // *** --- ***
            finalColor += surfaceColor * lightColor * diff * mat.diffuse;
            // Specular (Blinn-Phong)
            // *** Use dynamic light direction and view direction ***
            vmath_hpp::fvec3 halfwayDir = vmath_hpp::normalize(viewDir - lightDir);
            float spec = std::pow(std::max(0.0f, vmath_hpp::dot(normal, halfwayDir)), mat.shininess);
            // *** --- ***
            finalColor += lightColor * spec * mat.specular;
        }

        // Reflection (unchanged logic, but viewDir and hitPoint might be slightly different)
        if (mat.reflectivity > 0.0f) {
            vmath_hpp::fvec3 reflectDir = vmath_hpp::reflect(ray.direction, normal);
            Ray reflectionRay;
            reflectionRay.origin = hitPoint + normal * EPSILON;
            reflectionRay.direction = reflectDir;
            vmath_hpp::fvec3 reflectedColor = castRay(reflectionRay, depth + 1);
            finalColor = vmath_hpp::lerp(finalColor, reflectedColor, mat.reflectivity);
        }
        return finalColor;
    } else {
        return backgroundColor;
    }
}


// *** Heavily MODIFIED loop function ***
void Raytrace::loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) {
    t += dt;

    // --- Update Dynamic Elements ---

    // 1. Sphere Animation: Position (bobbing) + Color (hue cycle)
    spherePos = sphereBasePos + vmath_hpp::fvec3(0.0f, 0.3f * sin(t * 1.5f), 0.0f); // Bob up and down
    float hue = fmod(t * 40.0f, 360.0f); // Cycle hue over time (0-360 degrees)
    sphereMaterial.color = hsvToRgb(hue, 0.8f, 0.9f); // Update sphere material color (fixed saturation/value)

    // 2. Camera Animation: Orbit around the target
    camOrbitAngle += dt * 0.5f; // Radians per second orbit speed
    camPos.x = camTarget.x + camOrbitRadius * sin(camOrbitAngle);
    camPos.z = camTarget.z + camOrbitRadius * cos(camOrbitAngle); // Adjust Z based on target Z
    camPos.y = camHeight + 0.5f * sin(camOrbitAngle * 2.0f); // Add a little up/down movement to camera

    // 3. Grid Animation: Move diagonally
    float gridSpeed = 0.5f;
    gridOffset.x += dt * gridSpeed;
    gridOffset.y += dt * gridSpeed * 0.7f; // Move slightly different speed on Z

    // 4. Light Animation: Rotate light direction (like sun setting/rising)
    lightAngle += dt * 0.2f; // Radians per second light rotation speed
    float lightY = -0.8f + 0.4f * sin(lightAngle); // Move light up and down slightly
    float lightX = -0.5f * cos(lightAngle);
    float lightZ = -1.0f * sin(lightAngle);
    lightDir = vmath_hpp::normalize(vmath_hpp::fvec3(lightX, lightY, lightZ));

    // --- Rendering Setup ---
    const float aspectRatio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
    const float fov = 60.0f * (M_PI / 180.0f);
    const float tanHalfFov = std::tan(fov / 2.0f);

    // Update camera basis vectors based on new position and fixed target
    vmath_hpp::fvec3 camForward = vmath_hpp::normalize(camTarget - camPos);
    // Ensure robust 'up' vector calculation even when looking straight down/up
    vmath_hpp::fvec3 globalUp(0.0f, 1.0f, 0.0f);
    vmath_hpp::fvec3 camRight = vmath_hpp::normalize(vmath_hpp::cross(globalUp, camForward));
     // Check for gimbal lock (camForward aligned with globalUp)
    if (vmath_hpp::length2(camRight) < EPSILON * EPSILON) {
        // If looking straight up/down, use a different vector to cross with forward
         // e.g., cross with a vector slightly off the forward axis
        vmath_hpp::fvec3 alternativeRight = vmath_hpp::normalize(vmath_hpp::cross(vmath_hpp::fvec3(0.0f, 0.0f, 1.0f), camForward));
        if(vmath_hpp::length2(alternativeRight) < EPSILON * EPSILON){ // Still an issue? Use X axis
             camRight = vmath_hpp::normalize(vmath_hpp::cross(vmath_hpp::fvec3(1.0f, 0.0f, 0.0f), camForward));
        } else {
            camRight = alternativeRight;
        }
    }
    vmath_hpp::fvec3 camUp = vmath_hpp::normalize(vmath_hpp::cross(camForward, camRight));


    // AA Settings (unchanged)
    const int sqrtNumSamples = 1;
    const int numSamples = sqrtNumSamples * sqrtNumSamples;
    const float invNumSamples = 1.0f / static_cast<float>(numSamples);
    const float subPixelStep = 1.0f / static_cast<float>(sqrtNumSamples);

    // --- Pixel Rendering Loop ---
    
    for (int j = 0; j < HEIGHT; ++j) {
        for (int i = 0; i < WIDTH; ++i) {
            vmath_hpp::fvec3 accumulatedColor(0.0f, 0.0f, 0.0f);
            // Sub-pixel sampling loop (unchanged)
            for (int sy = 0; sy < sqrtNumSamples; ++sy) {
                for (int sx = 0; sx < sqrtNumSamples; ++sx) {
                    float sampleX = static_cast<float>(i) + (static_cast<float>(sx) + 0.5f) * subPixelStep;
                    float sampleY = static_cast<float>(j) + (static_cast<float>(sy) + 0.5f) * subPixelStep;
                    float ndcX = (2.0f * sampleX / static_cast<float>(WIDTH) - 1.0f);
                    float ndcY = (1.0f - 2.0f * sampleY / static_cast<float>(HEIGHT));
                    float cameraX = ndcX * aspectRatio * tanHalfFov;
                    float cameraY = ndcY * tanHalfFov;

                    Ray primaryRay;
                    primaryRay.origin = camPos; // Use updated camera position
                    // Use updated camera basis vectors
                    primaryRay.direction = vmath_hpp::normalize(
                        camRight * cameraX +
                        camUp * cameraY +
                        camForward
                    );
                    accumulatedColor += castRay(primaryRay, 0);
                } // End sx loop
            } // End sy loop

            vmath_hpp::fvec3 finalColor = accumulatedColor * invNumSamples;
            canvas.drawPixel(i, j, fvec3ToRGBA(finalColor));
        } // End i loop
    } // End j loop
}

const char* Raytrace::getName() const {
    return "Dynamic Raytracer"; // Updated name
}

