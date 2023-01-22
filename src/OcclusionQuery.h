#ifndef OCCLUSION_QUERY_H
#define OCCLUSION_QUERY_H

#include <glad/glad.h>

namespace OM3D {

    class OcclusionQuery
    {
    public:
        OcclusionQuery();

        void begin_query() const;

        void end_query();

        bool has_samples_passed() const { return samples_passed > 0; }

    private:
        GLuint query_id = 0; // OpenGL query object ID
        GLint samples_passed = 0; // Number of samples passed in last query
    };
}

#endif // !OCCLUSION_QUERY