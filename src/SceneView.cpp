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

void SceneView::update_frame() {
    _scene->update_frame(_camera);
}

void SceneView::render() const {
    if(_scene) {
        _scene->render(_camera);
    }
}

}
