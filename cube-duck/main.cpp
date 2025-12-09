#include "Window.h"
#include "Core.h"
#include "Math.h"
#include "PSOManager.h"
#include "GamesEngineeringBase.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "GEMObject.h"
#include "GEMAnimatedObject.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

// Create Pipeline Manager to access many strcuts

class Camera {
    public:
    Vec3 from;
    Vec3 to;
    Vec3 up;

    Camera() : from(25.0f, 55.0f, 80.0f), to(0, 1, 0), up(0, 1, 0) {}
    Camera(Vec3 from, Vec3 to, Vec3 up) : from(from), to(to), up(up) {}
};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    Window win;
    win.init(WINDOW_HEIGHT, WINDOW_WIDTH, 0, 0, "My Window");

    Core core;
    core.init(win.hwnd, win.windowWidth, win.windowHeight);
    
    ShaderManager* shaderManager = new ShaderManager(&core);
    Camera camera;
    
    Matrix viewMatrix;
    viewMatrix.setLookatMatrix(camera.from, camera.to, camera.up);

    Matrix projectionMatrix;
    float zFar = 1000.0f;
    float zNear = 0.01f;
    float FOV = 60.0f;
    projectionMatrix.setProjectionMatrix(zFar, zNear, FOV, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    Matrix WorldMatrix;
    WorldMatrix.setIdentity();

    GEMAnimatedObject duck(shaderManager, "models/Duck-white.gem");

    VertexShaderCBStaticModel vsCBStaticModel;
    VertexShaderCBAnimatedModel vsCBAnimatedModel;

    vsCBStaticModel.W = WorldMatrix;
    vsCBStaticModel.VP = (projectionMatrix.mul(viewMatrix));

    vsCBAnimatedModel.W = WorldMatrix;
    vsCBAnimatedModel.VP = (projectionMatrix.mul(viewMatrix));

    AnimationInstance animatedInstance;
    duck.init(&core, &vsCBAnimatedModel);

    animatedInstance.init(&duck.animatedModel->animation, 0);
    memcpy(vsCBAnimatedModel.bones, animatedInstance.matrices, sizeof(vsCBAnimatedModel.bones));
    
    //cube.init(&core, &vsCBStaticModel);
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

        Vec3 from = Vec3(11 * cos(time), 5, 11 * sinf(time));
        camera.from = from;
        viewMatrix.setLookatMatrix(camera.from, camera.to, camera.up);

        vsCBStaticModel.VP = (projectionMatrix.mul(viewMatrix));
        vsCBAnimatedModel.VP = (projectionMatrix.mul(viewMatrix));
        // acacia.draw(&core, &vsCBStaticModel);

        //cube.draw(&core, &vsCBStaticModel);

        vsCBStaticModel.W.setScaling(2.0f, 2.0f, 2.0f);
        vsCBAnimatedModel.W.setScaling(0.05f, 0.05f, 0.05f);
        // cube.draw(&core, &vsCBStaticModel);

        animatedInstance.update("idle variation", dt);
        //animatedInstance.animationFinished();
		if (animatedInstance.animationFinished() == true)
		{
			animatedInstance.resetAnimationTime();
		}
        memcpy(vsCBAnimatedModel.bones, animatedInstance.matrices, sizeof(vsCBAnimatedModel.bones));
        duck.draw(&core, &vsCBAnimatedModel); 

        // vsCBStaticModel.W.setTranslation(0, 0, 0);


        core.finishFrame();
    }

    core.flushGraphicsQueue();
    return 0;
}
