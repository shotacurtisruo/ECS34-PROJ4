#include "DijkstraPathRouter.h"
#include <gtest/gtest.h>

class DijkstraPathRouterTest : public ::testing::Test {
protected:
    std::unique_ptr<CDijkstraPathRouter> router;
    void SetUp() override {
        router = std::make_unique<CDijkstraPathRouter>(); 
    }
    void TearDown() override { 
        router.reset(); 
    }
};


TEST_F(DijkstraPathRouterTest, VertexTest) {
    auto v1 = router->AddVertex(std::string("meow"));
    EXPECT_EQ(router->VertexCount(), 1);
    EXPECT_EQ(std::any_cast<std::string>(router->GetVertexTag(v1)), "meow");
}


TEST_F(DijkstraPathRouterTest, ShortestPath) {
    auto v1 = router->AddVertex(std::string("meow"));
    auto v2 = router->AddVertex(std::string("lala"));
    auto v3 = router->AddVertex(std::string("cat"));
    auto v4 = router->AddVertex(std::string("teehee"));

    router->AddEdge(v1, v2, 5);
    router->AddEdge(v2, v3, 6);
    router->AddEdge(v1, v3, 7);

    std::vector<CPathRouter::TVertexID> path;
    EXPECT_EQ(router->FindShortestPath(v1, v3, path), 7);
    EXPECT_EQ(router->FindShortestPath(v2, v4, path), CDijkstraPathRouter::NoPathExists);
}