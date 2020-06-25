/*
 * FreeRTOS Wi-Fi for Infineon XMC4800 IoT Connectivity Kit V1.0.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Copyright (c) 2018, Infineon Technologies AG
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 * products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with
 * Infineon Technologies AG dave@infineon.com).
 */

/**
 * @file iot_wifi.c
 * @brief Wi-Fi Interface.
 */

/* Socket and Wi-Fi interface includes. */
#include "FreeRTOS.h"
#include "iot_wifi.h"

/* Wi-Fi configuration includes. */
#include "aws_wifi_config.h"
#include "limits.h"
#include "esp/esp.h"

#define CONNECT_BIT    (1 << 0)
#define DISCONNECT_BIT (1 << 1)
#define WIFI_TLS_INDEX 0

static void  WIFI_SetLastError(BaseType_t reason);
static WIFIErrorReason_t WIFI_mapVendorToWIFIReason(BaseType_t reason);

SemaphoreHandle_t xWiFiSemaphoreHandle; /**< Wi-Fi module semaphore. */
const TickType_t xSemaphoreWaitTicks = pdMS_TO_TICKS( wificonfigMAX_SEMAPHORE_WAIT_TIME_MS );

static TaskHandle_t xTaskToNotify = NULL;
static BaseType_t xIsWiFiInitialized = pdFALSE;

espr_t esp_callback_func(esp_cb_t* cb)
{
  switch (cb->type) {
    case ESP_CB_INIT_FINISH:
    {
      //configPRINT("Library initialized!\r\n");
      break;
    }

    case ESP_CB_RESET_FINISH:
    {
      //configPRINT("Device reset sequence finished!\r\n");
      break;
    }

    case ESP_CB_RESET:
    {
      //configPRINT("Device reset detected!\r\n");
      break;
    }

    case ESP_CB_WIFI_CONNECTED:
    {
      xTaskNotify(xTaskToNotify, CONNECT_BIT, eSetValueWithOverwrite);
      break;
    }

    case ESP_CB_WIFI_DISCONNECTED:
    {
      //configPRINT("Wifi Disconnected\r\n");
      //xSemaphoreGive(xWiFiConnectSemaphoreHandle);
      xTaskNotify(xTaskToNotify, DISCONNECT_BIT, eSetValueWithOverwrite);
      break;
    }
	default: break;
  }
  return espOK;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_On( void )
{
  if (xIsWiFiInitialized == pdFALSE)
  {
    if (esp_init(esp_callback_func, 1) != espOK)
    {
      return eWiFiFailure;
    }

    xWiFiSemaphoreHandle = xSemaphoreCreateBinary();
    if (xWiFiSemaphoreHandle == NULL)
    {
      return eWiFiFailure;
    }
    xSemaphoreGive(xWiFiSemaphoreHandle);

    xIsWiFiInitialized = pdTRUE;
  }

  return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Off( void )
{
  return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
  BaseType_t xResult;
  uint32_t ulNotifiedValue;
  WIFIReturnCode_t status = eWiFiFailure;

  if ((pxNetworkParams == NULL) || (pxNetworkParams->pcSSID == NULL))
  {
	return eWiFiFailure;
  }

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
    /* Store the handle of the calling task. */
    xTaskToNotify = xTaskGetCurrentTaskHandle();

    if (esp_sta_has_ip())
	{
	  if (esp_sta_quit(1) == espOK)
	  {
	      xResult = xTaskNotifyWait( pdFALSE,    /* Don't clear bits on entry. */
	                                 ULONG_MAX,        /* Clear all bits on exit. */
	                                 &ulNotifiedValue, /* Stores the notified value. */
									 xSemaphoreWaitTicks );

	      if (xResult == pdTRUE)
	      {
	    	if (ulNotifiedValue == DISCONNECT_BIT)
	    	{
	          status = eWiFiSuccess;
	        }
	      }
	  }
	}

    if (esp_sta_join(pxNetworkParams->pcSSID, pxNetworkParams->pcPassword, NULL, 0, 1) == espOK)
    {
      xResult = xTaskNotifyWait( pdFALSE,    /* Don't clear bits on entry. */
                                 ULONG_MAX,        /* Clear all bits on exit. */
                                 &ulNotifiedValue, /* Stores the notified value. */
								 xSemaphoreWaitTicks );

      if (xResult == pdTRUE)
      {
    	if (ulNotifiedValue == CONNECT_BIT)
    	{
          status = eWiFiSuccess;
        }
      }
    }

	/* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }

  return status;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Disconnect( void )
{
  BaseType_t xResult;
  uint32_t ulNotifiedValue;
  WIFIReturnCode_t status = eWiFiFailure;

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
	/* Store the handle of the calling task. */
	xTaskToNotify = xTaskGetCurrentTaskHandle();

	if (esp_sta_has_ip())
	{
      if (esp_sta_quit(1) == espOK)
      {
	      xResult = xTaskNotifyWait( pdFALSE,    /* Don't clear bits on entry. */
	                                 ULONG_MAX,        /* Clear all bits on exit. */
	                                 &ulNotifiedValue, /* Stores the notified value. */
									 xSemaphoreWaitTicks );

	      if (xResult == pdTRUE)
	      {
	    	if (ulNotifiedValue == DISCONNECT_BIT)
	    	{
	          status = eWiFiSuccess;
	        }
	      }
      }
	}
	else
	{
	  status = eWiFiSuccess;
	}
    /* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }

  return status;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Reset( void )
{
  WIFIReturnCode_t status = eWiFiFailure;

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
    if (esp_reset_with_delay(ESP_CFG_RESET_DELAY_DEFAULT, 1) == espOK)
    {
      status = eWiFiSuccess;
    }

    /* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }

  return status;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                            uint8_t ucNumNetworks )
{
  WIFIReturnCode_t status = eWiFiFailure;

  if ((pxBuffer == NULL) || (ucNumNetworks == 0))
  {
	return eWiFiFailure;
  }

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
    esp_ap_t APs[ESP_CFG_MAX_DETECTED_AP];
	size_t found_aps = 0;

	if (ucNumNetworks > ESP_CFG_MAX_DETECTED_AP)
	{
	  ucNumNetworks = ESP_CFG_MAX_DETECTED_AP;
	}

	if (esp_sta_list_ap(NULL, APs, ucNumNetworks, &found_aps, 1) == espOK)
    {
   	  for (int32_t i = 0; i < ucNumNetworks; ++i)
   	  {
   		memcpy(pxBuffer[i].cSSID, APs[i].ssid, wificonfigMAX_SSID_LEN);
   	    memcpy(pxBuffer[i].ucBSSID, APs[i].mac.mac, wificonfigMAX_BSSID_LEN);
   	    pxBuffer[i].cRSSI = APs[i].rssi;
   	    pxBuffer[i].xSecurity = APs[i].ecn;
   	    pxBuffer[i].cChannel = APs[i].ch;
   	  }

      status = eWiFiSuccess;
    }

    /* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }

  return status;
}
/*-----------------------------------------------------------*/

__STATIC_INLINE esp_mode_t aws_mode_conversion(WIFIDeviceMode_t xDeviceMode)
{
  switch (xDeviceMode)
  {
#if ESP_CFG_MODE_STATION
    case eWiFiModeStation:
      return ESP_MODE_STA;
      break;
#endif

#if ESP_CFG_MODE_ACCESS_POINT
    case eWiFiModeAP:
      return ESP_MODE_AP;
      break;
#endif

#if ESP_CFG_MODE_STATION_ACCESS_POINT
    case eWiFiModeP2P:
      return ESP_MODE_STA_AP;
      break;
#endif

    default:
      return ESP_MODE_NOT_SUPPORTED;
      break;
  }
}

WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{
  WIFIReturnCode_t status = eWiFiFailure;

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
    esp_mode_t mode = aws_mode_conversion(xDeviceMode);
    if (mode == ESP_MODE_NOT_SUPPORTED)
    {
	  status = eWiFiNotSupported;
    }

    if (esp_set_wifi_mode(mode, 1) == espOK)
    {
      status = eWiFiSuccess;
    }

    /* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }

  return status;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{
  if (pxDeviceMode == NULL)
  {
	return eWiFiFailure;
  }

  *pxDeviceMode = eWiFiModeStation;
  return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                  uint16_t * pusIndex )
{
  return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                  uint16_t usIndex )
{
  return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex )
{
  return eWiFiNotSupported;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS )
{
  char host_name[20];
  WIFIReturnCode_t status = eWiFiSuccess;
  int ret;

  /* Check params */
  if ((pucIPAddr == NULL) || (usCount == 0))
  {
    return eWiFiFailure;
  }

  sprintf(host_name, "%d.%d.%d.%d", pucIPAddr[0], pucIPAddr[1], pucIPAddr[2], pucIPAddr[3]);

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
    for (int32_t i = 0; i < usCount; ++i)
    {
      uint32_t time;
      ret = esp_ping(host_name, &time, 1);
      WIFI_SetLastError(ret);
      if ( ret != espOK )
      {
  	    status = eWiFiFailure;
            ESP_CFG_DBG_OUT( "WIFI_Ping returning %d \r\n", ret );
  	    break;
      }

      vTaskDelay(pdMS_TO_TICKS(ulIntervalMS));
    }

    /* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }


  return status;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetIP( uint8_t * pucIPAddr )
{
  esp_ip_t gw;
  esp_ip_t nm;

  WIFIReturnCode_t status = eWiFiFailure;

  if (pucIPAddr == NULL)
  {
	return eWiFiFailure;
  }

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
    if (esp_sta_getip((esp_ip_t *)pucIPAddr, &gw, &nm, 0, 1) == espOK)
    {
      status = eWiFiSuccess;
    }

    /* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }

  return status;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac )
{
  WIFIReturnCode_t status = eWiFiFailure;

  if (pucMac == NULL)
  {
	return eWiFiFailure;
  }

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
    if (esp_sta_getmac((esp_mac_t *)pucMac, 0, 1) == espOK)
    {
      status = eWiFiSuccess;
    }

    /* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }

  return status;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetHostIP( char * pcHost,
                                 uint8_t * pucIPAddr )
{
  WIFIReturnCode_t status = eWiFiFailure;

  if ((pcHost == NULL) || (pucIPAddr == NULL))
  {
	return eWiFiFailure;
  }

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
    if (esp_dns_getbyhostname(pcHost, (esp_ip_t *)pucIPAddr, 1) == espOK)
    {
      status = eWiFiSuccess;
    }

    /* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }

  return status;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StartAP( void )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StopAP( void )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

BaseType_t WIFI_IsConnected(void)
{
  BaseType_t status = 0;

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
	status = esp_sta_has_ip();

	/* Release semaphore */
	xSemaphoreGive(xWiFiSemaphoreHandle);
  }

  return status;
}

WIFIReturnCode_t WIFI_RegisterNetworkStateChangeEventCallback( IotNetworkStateChangeEventCallback_t xCallback  )
{
    /** Needs to implement dispatching network state change events **/
    return eWiFiNotSupported;
}

static WIFIErrorReason_t WIFI_mapVendorToWIFIReason(BaseType_t reason)
{
    WIFIErrorReason_t reason;
    switch (reason)
    {
        case ESP_OK:
            reason = 0;
            break;
        case ESP_FAIL:                 /*  -1      !< Generic esp_err_t code indicating failure */
        case ESP_ERR_NO_MEM:           /*  0x101   !< Out of memory */
        case ESP_ERR_INVALID_ARG:      /*  0x102   !< Invalid argument */
        case ESP_ERR_INVALID_STATE:    /*  0x103   !< Invalid state */
        case ESP_ERR_INVALID_SIZE:     /*  0x104   !< Invalid size */
        case ESP_ERR_NOT_FOUND:        /*  0x105   !< Requested resource not found */
        case ESP_ERR_NOT_SUPPORTED:    /*  0x106   !< Operation or feature not supported */
        case ESP_ERR_TIMEOUT:          /*  0x107   !< Operation timed out */
        case ESP_ERR_INVALID_RESPONSE: /*  0x108   !< Received response was invalid */
        case ESP_ERR_INVALID_CRC:      /*  0x109   !< CRC or checksum was invalid */
        case ESP_ERR_INVALID_VERSION:  /*  0x10A   !< Version was invalid */
        case ESP_ERR_INVALID_MAC:      /*  0x10B   !< MAC address was invalid */
        case ESP_ERR_WIFI_NOT_INIT:    /*!< WiFi driver was not installed by esp_wifi_init */
        case ESP_ERR_WIFI_NOT_STARTED: /*!< WiFi driver was not started by esp_wifi_start */
        case ESP_ERR_WIFI_NOT_STOPPED: /*!< WiFi driver was not stopped by esp_wifi_stop */
        case ESP_ERR_WIFI_IF:
        case ESP_ERR_WIFI_MODE:
        case ESP_ERR_WIFI_STATE:
        case ESP_ERR_WIFI_CONN:
        case ESP_ERR_WIFI_NVS:
        case ESP_ERR_WIFI_MAC:
        case ESP_ERR_WIFI_SSID:
        case ESP_ERR_WIFI_PASSWORD:
        case ESP_ERR_WIFI_WAKE_FAIL:
        case ESP_ERR_WIFI_WOULD_BLOCK:
        case ESP_ERR_WIFI_NOT_CONNECT:
        case ESP_ERR_WIFI_POST:
        case ESP_ERR_WIFI_INIT_STATE:
        case ESP_ERR_WIFI_STOP_STATE:
            reason = eWiFiNotOK;
            break;
        case ESP_ERR_WIFI_TIMEOUT:
            reason = eWiFiOperationTimeout;
        default:
            reason = eWiFiUnknown;
            break;

    }
    return reason;
}
static void  WIFI_SetLastError(BaseType_t reason)
{
#if (configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 && configNUM_THREAD_LOCAL_STORAGE_POINTERS > WIFI_TLS_INDEX)
    WIFIErrorReason_t error = WIFI_mapVendorToWIFIReason(reason);
    vTaskSetThreadLocalStoragePointer(NULL, WIFI_TLS_INDEX ,( void * ) error);
#endif
}

WIFIErrorReason_t WIFI_GetLastError()
{
    WIFIErrorReason_t  reason = eWiFiUnsupported

#if (configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 && configNUM_THREAD_LOCAL_STORAGE_POINTERS > WIFI_TLS_INDEX)

    reason = (WIFIErrorReason_t) pvTaskGetThreadLocalStoragePointer( NULL, WIFI_TLS_INDEX )

#endif

    return reason;
}
