#pragma once



namespace ag
{
    // 탈출문을 열기 위한 레버. 3개가 정해진 순서대로 올라가야 한다.
    // 순서 검증과 전체 초기화는 Game이 담당하고, 레버는 자신의 상태만 보관한다.
    enum class LeverId { Exhibition, Lab, Utilities };

    class Lever
    {
    public:
        LeverId id = LeverId::Exhibition;
        glm::vec3 position{ 0.0f };
        bool raised = false;

    private:
        
    };
}
