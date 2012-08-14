#include <SerialDongle.h>

/*********************************************
  Prototypes
*********************************************/

void startVisualizer(void);
void showStartingNwk(void);
void showStartedNwk(void);
void showAirTxStart(void);
void showAirTxStop(void);
void showSerialTx(void);
void showAirRxStart(void);
void stopVisualizer(void);
void stopVisualizer(void);


void startVisualizer(void)
{
  BSP_OpenLeds();
}

void showStartingNwk(void)
{
  BSP_ToggleLed(LED_RED);
  BSP_OffLed(LED_YELLOW);
  BSP_OffLed(LED_GREEN);
}

void showStartedNwk(void)
{
  BSP_OffLed(LED_RED);
  BSP_OffLed(LED_YELLOW);
  BSP_OffLed(LED_GREEN);
}

void showAirTxStart(void)
{
  BSP_OnLed(LED_GREEN);
}

void showAirTxStop(void)
{
  BSP_OffLed(LED_GREEN);
}

void showSerialTx(void)
{
  BSP_ToggleLed(LED_GREEN);
}

void showAirRxStart(void)
{
  BSP_OnLed(LED_YELLOW);
}

void showAirRxStop(void)
{
  BSP_OffLed(LED_YELLOW);
}

void stopVisualizer(void)
{
  BSP_CloseLeds();
}
