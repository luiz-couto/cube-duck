#include "Window.h"
#include "Core.h"
#include "Math.h"
#include "PSOManager.h"
#include "GamesEngineeringBase.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "GEMObject.h"
#include "GEMAnimatedObject.h"
#include "Camera.h"
#include "Cube.h"

// Create Pipeline Manager to access many strcuts

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    Window win;
    win.init(WINDOW_HEIGHT, WINDOW_WIDTH, 0, 0, "My Window");

    Core core;
    core.init(win.hwnd, win.windowWidth, win.windowHeight);
    
    ShaderManager* shaderManager = new ShaderManager(&core);
    Camera camera;

    Cube* cube = Cube::createGrassCube(shaderManager, &core);
    GEMAnimatedObject duck(shaderManager, "models/Duck-white.gem");
    
    AnimationInstance animatedInstance;
    VertexShaderCBAnimatedModel vsCBAnimatedModel;
    duck.init(&core, &vsCBAnimatedModel);

    //Cube cube(shaderManager, &GrassCubePixelShader);
    
    // VertexShaderCBAnimatedModel vsCBAnimatedModel;


    animatedInstance.init(&duck.animatedModel->animation, 0);
    memcpy(vsCBAnimatedModel.bones, animatedInstance.matrices, sizeof(vsCBAnimatedModel.bones));
    
    //cube.scale(0.01f);
    //acacia.init(&core, &vsCBStaticModel);
    //sphere.init(&core, &vsCBStaticModel);

    GamesEngineeringBase::Timer tim = GamesEngineeringBase::Timer();
    float time = 0.0f;

    while (true) {
        core.beginFrame();
        win.processMessages();
        if (win.keys[VK_ESCAPE] == 1) {
            break;
        }

        float mouseOffsetY = win.lastmousey - win.mousey;
        float mouseOffsetX = win.lastmousex - win.mousex;

        if (std::abs(mouseOffsetY) > 0.00001f) {
            camera.moveCameraY(mouseOffsetY * 0.01f);
        }

        if (win.mouseButtons[0] && std::abs(mouseOffsetX) > 0.00001f) {
            camera.rotate(-mouseOffsetX * 0.01f);
        } else if (std::abs(mouseOffsetX) > 0.00001f) {
            camera.moveCameraXZ(-mouseOffsetX * 0.01f);
        }

        win.lastmousex = win.mousex;
        win.lastmousey = win.mousey;

        float dt = tim.dt();
        time += dt;
        time = fmodf(time, 2 * 3.1415f); // Avoid precision issues

        // Vec3 from = Vec3(11 * cos(time), 5, 11 * sinf(time));
        // camera.from = from;

        core.beginRenderPass();

        // Draw first cube at origin
        cube->translate(Vec3(0.0f, 0.0f, 0.0f));
        cube->draw(&core, &camera);

        // Draw second cube at offset position
        cube->translate(Vec3(2.0f, 0.0f, 0.0f));
        cube->draw(&core, &camera);

        cube->translate(Vec3(0.0f, 0.0f, 2.0f));
        cube->draw(&core, &camera);

        cube->translate(Vec3(2.0f, 0.0f, 2.0f));
        cube->draw(&core, &camera);

        cube->translate(Vec3(1.0f, 2.0f, 1.0f));
        cube->draw(&core, &camera);

        animatedInstance.update("idle variation", dt);
		if (animatedInstance.animationFinished() == true)
		{
			animatedInstance.resetAnimationTime();
		}

        duck.scale(0.02f);
        duck.translate(Vec3(1.0f, 4.0f, 1.0f));
        duck.draw(&core, &camera, &animatedInstance);

        core.finishFrame();
    }

    core.flushGraphicsQueue();
    return 0;
}
