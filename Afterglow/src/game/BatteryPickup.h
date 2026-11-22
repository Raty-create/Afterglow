#pragma once



namespace ag
{
    // 맵에 흩어진 배터리. 줍기만 하면 여분 배터리 수가 늘어난다.
    class BatteryPickup
    {
    public:
        glm::vec3 position{ 0.0f };
        bool available = false;

    private:
        
    };
}
