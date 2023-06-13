#include "../vectors/vector2_int.h"

class TouchRaw
{
public:
    Vector2Int position = Vector2Int(0);
    int force = 0;
    int fingerId = 0;
    int screenIndex = 0;
};