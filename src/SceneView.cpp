#include "SceneView.h"

namespace OM3D {

SceneView::SceneView(Scene* scene) : _scene(scene) {
}

Camera& SceneView::camera() {
    return _camera;
}

const Camera& SceneView::camera() const {
    return _camera;
}

void SceneView::render() const {
    if(_scene) {
        _scene->render(_camera);
    }
}

void SceneView::compute_occlusion_query()
{
    if (_scene)
        _scene->compute_occlusion_query(_camera);
}

void SceneView::sort_front_to_back()
{
    if (_scene)
        _scene->sort_front_to_back(_camera.position());
}

}
