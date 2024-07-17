#include "sprites.h"

void drawSprite(MatrixPanel_I2S_DMA* matrix, int x, int y, const Sprite& sprite)
{
    for (int i = 0; i < sprite.height; i++)
    {
        for (int j = 0; j < sprite.width; j++)
        {
            matrix->drawPixel(x + j, y + i, sprite.data[i * sprite.width + j]);
        }
    }
}
