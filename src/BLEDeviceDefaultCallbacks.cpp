#define TAG "ScanCallback"
#include "BLEDevice.h"
#include  <Arduino.h>
#include "BLEScan.h"
#include "BLEAdvertisedDevice.h"
#include "BLEFreeRTOS.h"
#include "rpc_unified_log.h"
#include "seeed_rpcUnified.h"
#include "rtl_ble/ble_unified.h"
	
/** @brief  APP Return Result List */
T_APP_RESULT BLEScan::gapCallbackDefault(uint8_t cb_type, void *p_cb_data) {
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_LE_CB_DATA *p_data = (T_LE_CB_DATA *)p_cb_data;
    RPC_DEBUG("gapCallbackDefault into \r\n");
    switch (cb_type) {
        case GAP_MSG_LE_DATA_LEN_CHANGE_INFO: {
            RPC_INFO("GAP_MSG_LE_DATA_LEN_CHANGE_INFO: conn_id %d, tx octets 0x%x, max_tx_time 0x%x\r\n", p_data->p_le_data_len_change_info->conn_id, p_data->p_le_data_len_change_info->max_tx_octets, p_data->p_le_data_len_change_info->max_tx_time);
            break;
        }
        case GAP_MSG_LE_MODIFY_WHITE_LIST: {
            RPC_INFO("GAP_MSG_LE_MODIFY_WHITE_LIST: operation %d, cause 0x%x\r\n", p_data->p_le_modify_white_list_rsp->operation, p_data->p_le_modify_white_list_rsp->cause);
            break;
        }
        case GAP_MSG_LE_CONN_UPDATE_IND: {
            RPC_INFO("GAP_MSG_LE_CONN_UPDATE_IND: conn_id %d, conn_interval_max 0x%x, conn_interval_min 0x%x, conn_latency 0x%x,supervision_timeout 0x%x", p_data->p_le_conn_update_ind->conn_id, p_data->p_le_conn_update_ind->conn_interval_max, p_data->p_le_conn_update_ind->conn_interval_min, p_data->p_le_conn_update_ind->conn_latency, p_data->p_le_conn_update_ind->supervision_timeout);
            /* if reject the proposed connection parameter from peer device, use APP_RESULT_REJECT. */
            result = APP_RESULT_ACCEPT;
            break;
        }
        case GAP_MSG_LE_PHY_UPDATE_INFO: {
            RPC_INFO("GAP_MSG_LE_PHY_UPDATE_INFO:conn_id %d, cause 0x%x, rx_phy %d, tx_phy %d\r\n", p_data->p_le_phy_update_info->conn_id, p_data->p_le_phy_update_info->cause, p_data->p_le_phy_update_info->rx_phy, p_data->p_le_phy_update_info->tx_phy);
            break;
        }        
        case GAP_MSG_LE_REMOTE_FEATS_INFO: {
            uint8_t  remote_feats[8];
            if (p_data->p_le_remote_feats_info->cause == GAP_SUCCESS) {
                memcpy(remote_feats, p_data->p_le_remote_feats_info->remote_feats, 8);
                if (remote_feats[LE_SUPPORT_FEATURES_MASK_ARRAY_INDEX1] & LE_SUPPORT_FEATURES_LE_2M_MASK_BIT) {
                    RPC_DEBUG("GAP_MSG_LE_REMOTE_FEATS_INFO: support 2M\r\n");
                }
                if (remote_feats[LE_SUPPORT_FEATURES_MASK_ARRAY_INDEX1] & LE_SUPPORT_FEATURES_LE_CODED_PHY_MASK_BIT) {
                    RPC_DEBUG("GAP_MSG_LE_REMOTE_FEATS_INFO: support CODED\r\n");
                }
            }
            break;
        }
        case GAP_MSG_LE_SCAN_CMPL:
        {
            RPC_DEBUG("GAP_MSG_LE_SCAN_CMPL");
            m_semaphoreScanEnd.give();
             if(m_scanCompleteCB != nullptr) {
                 m_scanCompleteCB(m_scanResults);
             }
            break;
        }         
        case GAP_MSG_LE_SCAN_INFO: {
            RPC_DEBUG("GAP_MSG_LE_SCAN_INFO:adv_type 0x%x, bd_addr %02x:%02x:%02x:%02x:%02x:%02x, remote_addr_type %d, rssi %d, data_len %d",
                            p_data->p_le_scan_info->adv_type,
                            (p_data->p_le_scan_info->bd_addr)[5],
                            (p_data->p_le_scan_info->bd_addr)[4],
                            (p_data->p_le_scan_info->bd_addr)[3],
                            (p_data->p_le_scan_info->bd_addr)[2],
                            (p_data->p_le_scan_info->bd_addr)[1],
                            (p_data->p_le_scan_info->bd_addr)[0],
                            p_data->p_le_scan_info->remote_addr_type,
                            p_data->p_le_scan_info->rssi,
                            p_data->p_le_scan_info->data_len);
			RPC_DEBUG("GAP_MSG_LE_SCAN_INFO:\r\n");
            BLEAddress advertisedAddress(p_data->p_le_scan_info->bd_addr);			
            bool found = false;
			if (m_scanResults.m_vectorAdvertisedDevices.count(advertisedAddress.toString()) != 0) {
				found = true;
			}

            if (found && !m_wantDuplicates) {  // If we found a previous entry AND we don't want duplicates, then we are done.
				vTaskDelay(1);  // <--- allow to switch task in case we scan infinity and dont have new devices to report, or we are blocked here
				break;
			}

			BLEAdvertisedDevice *advertisedDevice = new BLEAdvertisedDevice();		
			advertisedDevice->setAddress(advertisedAddress);
			advertisedDevice->setRSSI(p_data->p_le_scan_info->rssi);
			advertisedDevice->parseAdvertisement(p_data);
			advertisedDevice->setScan(this);
			advertisedDevice->setAddressType(p_data->p_le_scan_info->remote_addr_type);
			
			if (!found) {   // If we have previously seen this device, don't record it again.
				m_scanResults.m_vectorAdvertisedDevices.insert(std::pair<std::string, BLEAdvertisedDevice*>(advertisedAddress.toString(), advertisedDevice));
			}			
			
            if (m_pAdvertisedDeviceCallbacks) {
                m_pAdvertisedDeviceCallbacks->onResult(*advertisedDevice);
            }
			if(found)
				delete advertisedDevice;
            break;
        }
        default:
            RPC_DEBUG("gapCallbackDefault: unhandled cb_type 0x%x\r\n", cb_type);
            break;
    }
    return result;
}
