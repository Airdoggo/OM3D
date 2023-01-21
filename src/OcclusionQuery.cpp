#include <OcclusionQuery.h>
#include <iostream>

namespace OM3D {

    OcclusionQuery::OcclusionQuery()
    {
        glGenQueries(1, &queryID);
    }

    void OcclusionQuery::free() const {
        glDeleteQueries(1, &queryID);
    }

    void OcclusionQuery::beginQuery() const {
        glBeginQuery(GL_SAMPLES_PASSED, queryID);
    }

    void OcclusionQuery::endQuery() {
        glEndQuery(GL_SAMPLES_PASSED);
        glGetQueryObjectiv(queryID, GL_QUERY_RESULT, &samplesPassed);
    }
}