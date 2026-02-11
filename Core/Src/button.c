#include "button.h"



BUTTON_CONTROL button[10]=
    {
        {GPIOC, GPIO_PIN_9, 0},			//버튼1
        {GPIOA, GPIO_PIN_5, 0},			//버튼2
        {GPIOB, GPIO_PIN_6, 0},			//버튼3
        {GPIOC, GPIO_PIN_7, 0},			//버튼4
        {GPIOA, GPIO_PIN_9, 0},			//버튼5
        {GPIOA, GPIO_PIN_8, 0},			//버튼6
        {GPIOB, GPIO_PIN_10, 0},		//버튼7
        {GPIOB, GPIO_PIN_4, 0},			//버튼8
        {GPIOB, GPIO_PIN_5, 0},			//버튼9
        {GPIOA, GPIO_PIN_10, 0},		//버튼10


    };






/* === 버튼 함수 ===
 * 하는 이유 : 버튼이 길게 눌리면 (컴퓨터 기준으로) 버튼이 토글이 되는 느낌을 받음
 * 내부 버튼을 누르고 한번 더 누르면 취소 버튼을 간주해서 dest_in에 수정이 되게해야 하는데
 * 기존의 디바운싱 코드로는 1,2번 입력된걸로 간주가 되어서 오류가 생겼음
 * 그래서 정말 정밀하게 딱 "1번" 입력되게 체크하는 새로운 함수가 필요해서 만듬
 */
bool buttonGetPressed(uint8_t num)		//취소 기능 구현을 위한 버튼 latch
{
    if(num >= 10) return false;		// 버튼 10개니까

    // level: 1=눌림(onState), 0=안눌림
    uint8_t st = (HAL_GPIO_ReadPin(button[num].port, button[num].number) == button[num].onState) ? 1 : 0;

    // 버튼별 상태
    static uint8_t  lastSetLevel[10] = {0};   // 마지막으로 확정된 레벨
    static uint8_t  lastRawLevel[10] = {0};   // 직전에 본 raw 레벨
    static uint32_t lastChange[10] = {0};     // raw가 바뀐 시각
    static uint8_t  latched[10] = {0};        // 눌림 1회 발생 락

    uint32_t now = HAL_GetTick();

    // raw 레벨이 바뀌면 디바운스 타이머 리셋
    if(st != lastRawLevel[num])
    {
        lastRawLevel[num] = st;
        lastChange[num] = now;
        return false;
    }

    // 일정 시간(디바운스) 동안 레벨이 유지되면 stable로 확정
    if((now - lastChange[num]) >= 30) // 30ms 디바운스
    {
        if(lastSetLevel[num] != st)
        {
            lastSetLevel[num] = st;
        }

        // "눌림 stableLevel=1"이 되었을 때, latched가 0이면 1회 true
        if(lastSetLevel[num] == 1 && latched[num] == 0)
        {
            latched[num] = 1;
            return true;  // ★ 딱 한 번만 발생
        }

        // 버튼을 떼면(lv=0) latch 해제 -> 다음 눌림 받을 준비
        if(lastSetLevel[num] == 0)
        {
            latched[num] = 0;
        }
    }

    return false;
}



//bool buttonGetPressed(uint8_t num)
//{
//  static uint32_t prevTime = 0;
//  bool ret = false;
//
//  if(HAL_GPIO_ReadPin(button[num].port,button[num].number) == button[num].onState)
//  {
//    uint32_t currTime = HAL_GetTick();
//    if(currTime - prevTime > 100)
//    {
//      if(HAL_GPIO_ReadPin(button[num].port,button[num].number) == button[num].onState)
//      {
//            ret = true;
//      }
//      prevTime = currTime;
//    }
//  }
//  return ret;
//}


