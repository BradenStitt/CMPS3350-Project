

extern void display_border(int xres, int yres);

class Platform
{
public:
    float pos[2];
    float width;
    float height;

    Platform()
    {
        pos[0] = 300.0f;
        pos[1] = 20.0f;
        width = 50.0f;
        height = 8.0f;
    }
}