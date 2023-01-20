#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include <Scene.h>
#include <Camera.h>

namespace OM3D {

class SceneView {
    public:
        SceneView(Scene* scene = nullptr);

        Camera& camera();
        const Camera& camera() const;

        void update_frame();

        void render() const;
        void compute_lights() const;

    private:
        Scene* _scene = nullptr;
        Camera _camera;

};

}

#endif // SCENEVIEW_H
