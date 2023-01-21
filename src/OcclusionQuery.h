#ifndef OCCLUSION_QUERY_H
#define OCCLUSION_QUERY_H

#include <glad/glad.h>

namespace OM3D {

    class OcclusionQuery
    {
    public:
        OcclusionQuery();
        void free() const;

        /**
            * Begins occlusion query. Until the query is ended, samples
            * that pass the rendering pipeline are counted.
            */
        void beginQuery() const;

        /**
            * Ends occlusion query and caches the result - number
            * of samples that passed the rendering pipeline.
            */
        void endQuery();

        /**
            * Gets number of samples that have passed the rendering pipeline.
            */
        GLint getNumSamplesPassed() const { return samplesPassed; }

        /**
        * Helper method that returns if any samples have passed the rendering pipeline.
        */
        bool anySamplesPassed() const { return samplesPassed > 0; }

    private:
        GLuint queryID = 0; // OpenGL query object ID
        GLint samplesPassed = 0; // Number of samples passed in last query
    };
}

#endif // !OCCLUSION_QUERY