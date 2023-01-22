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

        void render() const;
        void compute_occlusion_query();
        void sort_front_to_back();

        Scene* _scene = nullptr;
    private:
        Camera _camera;

};

}

#endif // SCENEVIEW_H
