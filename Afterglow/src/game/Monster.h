#pragma once



namespace ag
{
    // 괴물의 세 가지 상태. 빛에 끌린다는 설정이 핵심이다.
    enum class MonsterState
    {
        Patrol,     // 정해진 웨이포인트 순찰
        Attracted,  // 감지한 빛 방향으로 이동 (가까워지면 가속)
        Search      // 마지막 빛 위치 주변 수색 후 순찰 복귀
    };

    // 시력이 아니라 빛을 감지한다.
    // 감지 거리는 랜턴 밝기에서 오고, 벽에 가려지면 감지 반경이 줄어든다.
    class Monster
    {
    public:
        

    private:
        
    };
}
