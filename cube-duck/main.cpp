#include "Window.h"
#include "Core.h"
#include "Math.h"
#include "PSOManager.h"
#include "GamesEngineeringBase.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "GEMObject.h"
#include "GEMAnimatedObject.h"
#include "Cube.h"
#include "Camera.h"

// Create Pipeline Manager to access many strcuts

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    Window win;
    win.init(WINDOW_HEIGHT, WINDOW_WIDTH, 0, 0, "My Window");

    Core core;
    core.init(win.hwnd, win.windowWidth, win.windowHeight);
    
    ShaderManager* shaderManager = new ShaderManager(&core);
    Camera camera;

    // GEMAnimatedObject duck(shaderManager, "models/Duck-white.gem");
    // AnimationInstance animatedInstance;
    // duck.init(&core, &vsCBAnimatedModel);

    Cube cube(shaderManager, &GrassCubePixelShader);
    cube.init(&core);
    
    // VertexShaderCBAnimatedModel vsCBAnimatedModel;


    //animatedInstance.init(&duck.animatedModel->animation, 0);
    //memcpy(vsCBAnimatedModel.bones, animatedInstance.matrices, sizeof(vsCBAnimatedModel.bones));
    
    cube.scale(2.0f);
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

        float dt = tim.dt();
        time += dt;
        time = fmodf(time, 2 * 3.1415f); // Avoid precision issues

        //Vec3 from = Vec3(11 * cos(time), 5, 11 * sinf(time));
        //camera.from = from;

        //vsCBAnimatedModel.VP = (projectionMatrix.mul(viewMatrix));
        // acacia.draw(&core, &vsCBStaticModel);

        //cube.draw(&core, &vsCBStaticModel);

        //vsCBAnimatedModel.W.setScaling(0.05f, 0.05f, 0.05f);
        cube.draw(&core, &camera);

        // animatedInstance.update("idle variation", dt);
        // //animatedInstance.animationFinished();
		// if (animatedInstance.animationFinished() == true)
		// {
		// 	animatedInstance.resetAnimationTime();
		// }
        // memcpy(vsCBAnimatedModel.bones, animatedInstance.matrices, sizeof(vsCBAnimatedModel.bones));
        // duck.draw(&core, &vsCBAnimatedModel); 

        // vsCBStaticModel.W.setTranslation(0, 0, 0);


        core.finishFrame();
    }

    core.flushGraphicsQueue();
    return 0;
}
