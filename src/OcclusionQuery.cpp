#include <OcclusionQuery.h>
#include <iostream>

namespace OM3D {

    OcclusionQuery::OcclusionQuery()
    {
        glGenQueries(1, &query_id);
    }

    void OcclusionQuery::begin_query() const {
        glBeginQuery(GL_SAMPLES_PASSED, query_id);
    }

    void OcclusionQuery::end_query() {
        glEndQuery(GL_SAMPLES_PASSED);
        glGetQueryObjectiv(query_id, GL_QUERY_RESULT, &samples_passed);
    }
}