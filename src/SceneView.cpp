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

void SceneView::compute_lights(const Material &sun_light_material, const Material &point_light_material) const {
    if(_scene) {
        _scene->compute_lights(_camera, sun_light_material, point_light_material);
    }
}

void SceneView::compute_occlusion_query()
{
    _scene->compute_occlusion_query(_camera);
}

void SceneView::sort_front_to_back()
{
    _scene->sort_front_to_back(_camera.position());
}

}
