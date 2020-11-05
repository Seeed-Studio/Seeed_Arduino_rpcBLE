#define TAG "device"
#include <map>     // Part of C++ Standard library
#include <sstream> // Part of C++ Standard library
#include <iomanip> // Part of C++ Standard library
#include "rpc_unified_log.h"
#include "BLEDevice.h"
#include <Arduino.h>

/**
 * Singletons for the BLEDevice.
 */

bool initialized = false;
BLEScan *BLEDevice::_pBLEScan = nullptr;
BLEClient *BLEDevice::m_pClient = nullptr;
BLEServer *BLEDevice::m_pServer = nullptr;
BLEAdvertising *BLEDevice::m_bleAdvertising = nullptr;
bool BLEDevice::ble_start_flags = false;
std::string BLEDevice::ble_name = "";
T_CLIENT_ID BLEClient::m_gattc_if = 0;
uint16_t BLEDevice::m_localMTU = 23; // not sure if this variable is useful
std::map<uint16_t, conn_status_t> BLEDevice::m_connectedClientsMap;
void ble_conn_state_evt_handler(uint8_t conn_id, T_GAP_CONN_STATE new_state, uint16_t disc_cause);
void ble_dev_state_evt_handler(T_GAP_DEV_STATE new_state, uint16_t cause);
void ble_param_update_evt_handler(uint8_t conn_id, uint8_t status, uint16_t cause);
void ble_mtu_info_evt_handler(uint8_t conn_id, uint16_t mtu_size);
void ble_authen_state_evt_handler(uint8_t conn_id, uint8_t new_state, uint16_t cause);

T_GAP_DEV_STATE ble_gap_dev_state = {0, 0, 0, 0, 0}; /**< GAP device state */
T_GAP_CONN_STATE ble_gap_conn_state = GAP_CONN_STATE_DISCONNECTED;
T_APP_LINK ble_clinet_link_table[BLE_LE_MAX_LINKS];

/**
 * @brief Create a new instance of a server.
 * @return A new instance of the server.
 */
/* STATIC */ BLEServer *BLEDevice::createServer()
{
    m_pServer = new BLEServer();
    RPC_DEBUG("BLE create Server\n\r");
    return m_pServer;
} // createServer

/**
 * @brief Create a new instance of a client.
 * @return A new instance of the client.
 */
/* STATIC */ BLEClient *BLEDevice::createClient()
{
    m_pClient = new BLEClient();
    return m_pClient;
} // createClient

BLEClient *BLEDevice::getClient()
{
    return m_pClient;
}

/**
 * @brief Retrieve the Scan object that we use for scanning.
 * @return The scanning object reference.  This is a singleton object.  The caller should not
 * try and release/delete it.
 */
/* STATIC */ BLEScan *BLEDevice::getScan()
{
    if (_pBLEScan == nullptr)
    {
        _pBLEScan = new BLEScan();
    }
    return _pBLEScan;
} // getScan

/* STATIC */ BLEServer *BLEDevice::getServer()
{
    return m_pServer;
} // getScan

BLEAdvertising *BLEDevice::getAdvertising()
{
    if (m_bleAdvertising == nullptr)
    {
        m_bleAdvertising = new BLEAdvertising();
    }
    return m_bleAdvertising;
}

void BLEDevice::startAdvertising()
{
    getAdvertising()->start();
} // startAdvertising

void BLEDevice::stopAdvertising()
{
    getAdvertising()->stop();
} // stopAdvertising
  /**
 * @brief Get the BLE device address.
 * @return The BLE device address.
 */
/* STATIC*/ BLEAddress BLEDevice::getAddress()
{
    uint8_t bt_addr[6];
    gap_get_param(GAP_PARAM_BD_ADDR, bt_addr);
    return BLEAddress(bt_addr);
} // getAddress

/**
 * @brief Get the value of a characteristic of a service on a remote device.
 * @param [in] bdAddress
 * @param [in] serviceUUID
 * @param [in] characteristicUUID
 */
/* STATIC */ std::string BLEDevice::getValue(BLEAddress bdAddress, BLEUUID serviceUUID, BLEUUID characteristicUUID)
{
    BLEClient *pClient = createClient();
    pClient->connect(bdAddress);
    std::string ret = pClient->getValue(serviceUUID, characteristicUUID);
    pClient->disconnect();
    return ret;
} // getValue

/**
 * @brief Set the value of a characteristic of a service on a remote device.
 * @param [in] bdAddress
 * @param [in] serviceUUID
 * @param [in] characteristicUUID
 */
/* STATIC */ void BLEDevice::setValue(BLEAddress bdAddress, BLEUUID serviceUUID, BLEUUID characteristicUUID, std::string value)
{
    BLEClient *pClient = createClient();
    pClient->connect(bdAddress);
    pClient->setValue(serviceUUID, characteristicUUID, value);
    pClient->disconnect();
} // setValue

/**
 * @brief Return a string representation of the nature of this device.
 * @return A string representation of the nature of this device.
 */
/* STATIC */ std::string BLEDevice::toString()
{
    std::string res = "BD Address: " + getAddress().toString();
    return res;
} // toString

/**
 * @brief Add an entry to the BLE white list.
 * @param [in] address The address to add to the white list.
 */
void BLEDevice::whiteListAdd(T_GAP_WHITE_LIST_OP operation, uint8_t *bd_addr,
                             T_GAP_REMOTE_ADDR_TYPE bd_type)
{
    operation = GAP_WHITE_LIST_OP_ADD;
    le_modify_white_list(operation, bd_addr, bd_type);
} // whiteListAdd

/**
 * @brief Remove an entry from the BLE white list.
 * @param [in] address The address to remove from the white list.
 */
void BLEDevice::whiteListRemove(T_GAP_WHITE_LIST_OP operation, uint8_t *bd_addr,
                                T_GAP_REMOTE_ADDR_TYPE bd_type)
{
    operation = GAP_WHITE_LIST_OP_REMOVE;
    le_modify_white_list(operation, bd_addr, bd_type);
} // whiteListRemove

/*
 * @brief Setup local mtu that will be used to negotiate mtu during request from client peer
 * @param [in] mtu Value to set local mtu, should be larger than 23 and lower or equal to 517
 */
void BLEDevice::setMTU(uint16_t mtu)
{
    gap_config_max_mtu_size(mtu);
    m_localMTU = mtu;
}

uint16_t BLEDevice::getMTU()
{
    return m_localMTU;
}

bool BLEDevice::getInitialized()
{
    return initialized;
}

BLEClient *BLEDevice::getClientByGattIf(uint16_t conn_id)
{
    return (BLEClient *)m_connectedClientsMap.find(conn_id)->second.peer_device;
}

/**
 * @brief Initialize the %BLE environment.
 * @param deviceName The device name of the device.
 */
/* STATIC */ void BLEDevice::init(std::string deviceName)
{
    if (!initialized)
    {
        initialized = true; // Set the initialization flag to ensure we are only initialized once.
                            /*
	 * Bluetooth controller initialization
	 */
        ble_init();
        ble_server_init(BLE_SERVER_MAX_APPS);
        ble_client_init(BLE_CLIENT_MAX_APPS);

        // /* GAP Bond Manager parameters */
        // uint8_t auth_pair_mode = GAP_PAIRING_MODE_PAIRABLE;
        // uint16_t auth_flags = GAP_AUTHEN_BIT_NONE;
        // uint8_t auth_io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
        // uint8_t auth_oob = false;
        // uint8_t auth_use_fix_passkey = false;
        // uint32_t auth_fix_passkey = 0;
        // uint8_t auth_sec_req_enable = false;
        // uint16_t auth_sec_req_flags = GAP_AUTHEN_BIT_BONDING_FLAG;

        // /* Setup the GAP Bond Manager */
        // gap_set_param(GAP_PARAM_BOND_PAIRING_MODE, sizeof(auth_pair_mode), &auth_pair_mode);
        // gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(auth_flags), &auth_flags);
        // gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(auth_io_cap), &auth_io_cap);
        // gap_set_param(GAP_PARAM_BOND_OOB_ENABLED, sizeof(auth_oob), &auth_oob);
        // le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY, sizeof(auth_fix_passkey), &auth_fix_passkey);
        // le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY_ENABLE, sizeof(auth_use_fix_passkey),
        //                   &auth_use_fix_passkey);
        // le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, sizeof(auth_sec_req_enable), &auth_sec_req_enable);
        // le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_REQUIREMENT, sizeof(auth_sec_req_flags),
        //                   &auth_sec_req_flags);
        
        // uint8_t  slave_init_mtu_req = true;
        // le_set_gap_param(GAP_PARAM_SLAVE_INIT_GATT_MTU_REQ, sizeof(slave_init_mtu_req),&slave_init_mtu_req);


        /*
     *  Register the Bluetooth callback function
     */
        le_register_app_cb(BLEDevice::gapEventHandler);
        le_register_msg_handler(BLEDevice::ble_handle_gap_msg);
        le_register_gattc_cb(BLEDevice::gattClientEventHandler);
        le_register_gatts_cb(BLEDevice::gattServerEventHandler);

        /*
     * Set Bluetooth device  name
     */
        ble_name = deviceName;
        uint8_t device_name[GAP_DEVICE_NAME_LEN] = {0};
        memcpy(device_name, deviceName.c_str(), GAP_DEVICE_NAME_LEN);
        le_set_gap_param(GAP_PARAM_DEVICE_NAME, GAP_DEVICE_NAME_LEN, device_name);
    }
    RPC_DEBUG("BLE init success\n\r");
    return;
} // init

/**
 * @brief de-Initialize the %BLE environment.
 * @param release_memory release the internal BT stack memory
 */
/* STATIC */ void BLEDevice::deinit()
{
    if (!initialized)
        return;
    ble_deinit();
}

/**
 * @brief Handle GAP events.
 */
/* STATIC */ T_APP_RESULT BLEDevice::gapEventHandler(
    uint8_t cb_type,
    void *p_cb_data)
{
    T_APP_RESULT ret = APP_RESULT_SUCCESS;

    if (BLEDevice::_pBLEScan != nullptr)
    {
        BLEDevice::getScan()->gapCallbackDefault(cb_type, p_cb_data);
    }

    if (BLEDevice::m_pClient != nullptr)
    {
        BLEDevice::m_pClient->handleGAPEvent(cb_type, p_cb_data);
    }

    if (m_bleAdvertising != nullptr)
    {
        BLEDevice::getAdvertising()->handleGAPEvent(cb_type, p_cb_data);
    }
    return ret;
} // gapEventHandler

/**
 * @brief Handle GATT client events.
 *
 * Handler for the GATT client events.
 *
 * @param [in] event
 * @param [in] gattc_if
 * @param [in] param
 */
/* STATIC */ T_APP_RESULT BLEDevice::gattClientEventHandler(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    for (auto &myPair : BLEDevice::getPeerDevices(true))
    {
        conn_status_t conn_status = (conn_status_t)myPair.second;
        if (((BLEClient *)conn_status.peer_device)->getGattcIf() == client_id || ((BLEClient *)conn_status.peer_device)->getGattcIf() == 0xff || client_id == 0xff)
        {
            ((BLEClient *)conn_status.peer_device)->clientCallbackDefault(client_id, conn_id, p_data);
        }
    }

    return result;
} // gattClientEventHandler

/* multi connect support */
/* requires a little more work */
std::map<uint16_t, conn_status_t> BLEDevice::getPeerDevices(bool _client)
{
    return m_connectedClientsMap;
}

void BLEDevice::addPeerDevice(void *peer, bool _client, uint16_t conn_id)
{
    conn_status_t status = {
        .peer_device = peer,
        .connected = true,
        .mtu = 247};

    m_connectedClientsMap.insert(std::pair<uint16_t, conn_status_t>(conn_id, status));
}

void BLEDevice::updatePeerDevice(void *peer, bool _client, uint16_t conn_id)
{
    std::map<uint16_t, conn_status_t>::iterator it = m_connectedClientsMap.find(0xff);
    if (it != m_connectedClientsMap.end())
    {
        std::swap(m_connectedClientsMap[conn_id], it->second);
        m_connectedClientsMap.erase(it);
    }
    else
    {
        it = m_connectedClientsMap.find(conn_id);
        if (it != m_connectedClientsMap.end())
        {
            conn_status_t _st = it->second;
            _st.peer_device = peer;
            std::swap(m_connectedClientsMap[conn_id], _st);
        }
    }
}

void BLEDevice::removePeerDevice(uint16_t conn_id, bool _client)
{
    if (m_connectedClientsMap.find(conn_id) != m_connectedClientsMap.end())
        m_connectedClientsMap.erase(conn_id);
}

/**
 * @brief Handle GATT server events.
 *
 * @param [in] event The event that has been newly received.
 * @param [in] gatts_if The connection to the GATT interface.
 * @param [in] param Parameters for the event.
 */
/* STATIC */ T_APP_RESULT BLEDevice::gattServerEventHandler(
    T_SERVER_ID service_id,
    void *p_data)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    if (BLEDevice::m_pServer != nullptr)
    {
        RPC_DEBUG("into device :: gattServerEventHandler\n\r");

        BLEDevice::m_pServer->handleGATTServerEvent(service_id, p_data);
    }
    return APP_RESULT_SUCCESS;

} // gattServerEventHandler

/**
 * @brief    All the BT GAP MSG are pre-handled in this function.
 * @note     Then the event handling function shall be called according to the
 *           subtype of T_IO_MSG
 * @param[in] p_gap_msg Pointer to GAP msg
 * @return   void
 */
void BLEDevice::ble_handle_gap_msg(T_IO_MSG *p_gap_msg)
{
    T_LE_GAP_MSG gap_msg;
    uint8_t conn_id;
    memcpy(&gap_msg, &p_gap_msg->u.param, sizeof(p_gap_msg->u.param));

    RPC_DEBUG("ble_handle_gap_msg: subtype %d\n\r", p_gap_msg->subtype);

    switch (p_gap_msg->subtype)
    {
    case GAP_MSG_LE_DEV_STATE_CHANGE:
    {
        RPC_DEBUG("GAP_MSG_LE_DEV_STATE_CHANGE\n\r");
        ble_dev_state_evt_handler(gap_msg.msg_data.gap_dev_state_change.new_state,
                                  gap_msg.msg_data.gap_dev_state_change.cause);
    }
    break;
    case GAP_MSG_LE_CONN_STATE_CHANGE:
    {

        RPC_DEBUG("GAP_MSG_LE_CONN_STATE_CHANGE\n\r");
        ble_conn_state_evt_handler(gap_msg.msg_data.gap_conn_state_change.conn_id,
                                   (T_GAP_CONN_STATE)gap_msg.msg_data.gap_conn_state_change.new_state,
                                   gap_msg.msg_data.gap_conn_state_change.disc_cause);
    }
    break;
    case GAP_MSG_LE_CONN_PARAM_UPDATE:
    {
        RPC_DEBUG("GAP_MSG_LE_CONN_PARAM_UPDATE\n\r");
        ble_param_update_evt_handler(gap_msg.msg_data.gap_conn_param_update.conn_id,
                                     gap_msg.msg_data.gap_conn_param_update.status,
                                     gap_msg.msg_data.gap_conn_param_update.cause);
    }
    break;
    case GAP_MSG_LE_CONN_MTU_INFO:
    {
        RPC_DEBUG("GAP_MSG_LE_CONN_MTU_INFO\n\r");
        ble_mtu_info_evt_handler(gap_msg.msg_data.gap_conn_mtu_info.conn_id,
                                 gap_msg.msg_data.gap_conn_mtu_info.mtu_size);
    }
    break;
    case GAP_MSG_LE_AUTHEN_STATE_CHANGE:
    {
        RPC_DEBUG("GAP_MSG_LE_AUTHEN_STATE_CHANGE\n\r");
        ble_authen_state_evt_handler(gap_msg.msg_data.gap_authen_state.conn_id,
                                     gap_msg.msg_data.gap_authen_state.new_state,
                                     gap_msg.msg_data.gap_authen_state.status);
    }
    break;
    case GAP_MSG_LE_BOND_PASSKEY_DISPLAY:
    {
        RPC_DEBUG("GAP_MSG_LE_BOND_PASSKEY_DISPLAY\n\r");
        conn_id = gap_msg.msg_data.gap_bond_just_work_conf.conn_id;
        le_bond_just_work_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
    }
    break;
    case GAP_MSG_LE_BOND_PASSKEY_INPUT:
    {
        RPC_DEBUG("GAP_MSG_LE_BOND_PASSKEY_INPUT\n\r");
        uint32_t display_value = 0;
        conn_id = gap_msg.msg_data.gap_bond_passkey_display.conn_id;
        le_bond_get_display_key(conn_id, &display_value);
        RPC_DEBUG("GAP_MSG_LE_BOND_PASSKEY_DISPLAY:passkey %d\n\r", display_value);
        le_bond_passkey_display_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
        RPC_DEBUG("GAP_MSG_LE_BOND_PASSKEY_DISPLAY:passkey %d\n\r", display_value);
    }
    break;
    case GAP_MSG_LE_BOND_OOB_INPUT:
    {
        RPC_DEBUG("GAP_MSG_LE_BOND_OOB_INPUT\n\r");
        uint8_t oob_data[GAP_OOB_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        conn_id = gap_msg.msg_data.gap_bond_oob_input.conn_id;
        RPC_DEBUG("GAP_MSG_LE_BOND_OOB_INPUT\r\n\n\r");
        le_bond_set_param(GAP_PARAM_BOND_OOB_DATA, GAP_OOB_LEN, oob_data);
        le_bond_oob_input_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
    }
    break;
    case GAP_MSG_LE_BOND_USER_CONFIRMATION:
    {
        RPC_DEBUG("GAP_MSG_LE_BOND_USER_CONFIRMATION\n\r");
        uint32_t display_value = 0;
        conn_id = gap_msg.msg_data.gap_bond_user_conf.conn_id;
        le_bond_get_display_key(conn_id, &display_value);
        RPC_DEBUG("GAP_MSG_LE_BOND_USER_CONFIRMATION: passkey %ld\r\n\n\r", display_value);
        le_bond_user_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
    }
    break;
    case GAP_MSG_LE_BOND_JUST_WORK:
    {
        RPC_DEBUG("GAP_MSG_LE_BOND_JUST_WORK\n\r");
        conn_id = gap_msg.msg_data.gap_bond_just_work_conf.conn_id;
        le_bond_just_work_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
        RPC_DEBUG("GAP_MSG_LE_BOND_JUST_WORK\r\n\n\r");
    }
    break;
    default:
        RPC_DEBUG("gapMsgHandlerDefault: unknown subtype %d\n\r", p_gap_msg->subtype);
        break;
    }
}

/**
 * @brief    Handle msg GAP_MSG_LE_CONN_STATE_CHANGE
 * @note     All the gap conn state events are pre-handled in this function.
 *           Then the event handling function shall be called according to the new_state
 * @param[in] conn_id Connection ID
 * @param[in] new_state  New gap connection state
 * @param[in] disc_cause Use this cause when new_state is GAP_CONN_STATE_DISCONNECTED
 * @return   void
 */
void ble_conn_state_evt_handler(uint8_t conn_id, T_GAP_CONN_STATE new_state, uint16_t disc_cause)
{
    switch (new_state)
    {
    case GAP_CONN_STATE_DISCONNECTED:
    {
        if ((disc_cause != (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE)) && (disc_cause != (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)))
        {
            RPC_DEBUG("connection lost cause 0x%x\n\r", disc_cause);
        }
        if (BLEDevice::getServer() != nullptr)
        {
            if (BLEDevice::getServer()->getCallbacks() != nullptr)
            {
                BLEDevice::getServer()->getCallbacks()->onDisconnect(BLEDevice::getServer());
            }
            le_adv_start();
        }

        break;
    }
    case GAP_CONN_STATE_CONNECTED:
    {
        if (BLEDevice::getServer() != nullptr)
        {
            BLEDevice::getServer()->addPeerDevice((void *)BLEDevice::getServer(), false, conn_id);
            BLEDevice::getServer()->setConnectedCount();
            if (BLEDevice::getServer()->getCallbacks() != nullptr)
            {
                BLEDevice::getServer()->getCallbacks()->onConnect(BLEDevice::getServer());
            }
        }

        if (BLEDevice::getClient() != nullptr)
        {
            BLEDevice::updatePeerDevice(BLEDevice::getClient(), true, BLEDevice::getClient()->getGattcIf());
        }
        break;
    }
    default:
        break;
    }
    //}
    // ble_gap_conn_state = new_state;
}

/**
 * @brief    Handle msg GAP_MSG_LE_DEV_STATE_CHANGE
 * @note     All the gap device state events are pre-handled in this function.
 *           Then the event handling function shall be called according to the new_state
 * @param[in] new_state  New gap device state
 * @param[in] cause GAP device state change cause
 * @return   void
 */
void ble_dev_state_evt_handler(T_GAP_DEV_STATE new_state, uint16_t cause)
{
    RPC_DEBUG("ble_dev_state_evt_handler: init state %d, adv state %d, cause 0x%x\n\r", new_state.gap_init_state, new_state.gap_adv_state, cause);
    if (ble_gap_dev_state.gap_init_state != new_state.gap_init_state)
    {
        if (new_state.gap_init_state == GAP_INIT_STATE_STACK_READY)
        {
            RPC_DEBUG("GAP stack ready\n\r");
            // BLE stack is ready
            uint8_t bt_addr[6];
            gap_get_param(GAP_PARAM_BD_ADDR, bt_addr);
            RPC_DEBUG("[BLE Device] Local BT addr: %2x:%2x:%2x:%2x:%2x:%2x\n\r", bt_addr[5], bt_addr[4], bt_addr[3], bt_addr[2], bt_addr[1], bt_addr[0]);
        }
    }

    // Assign different tasks according to different roles
    // As a Client
    if (BLEDevice::getClient() != nullptr)
    {
        if (ble_gap_dev_state.gap_scan_state != new_state.gap_scan_state)
        {
            if (new_state.gap_scan_state == GAP_SCAN_STATE_IDLE)
            {
                RPC_DEBUG("[BLE Device] GAP scan stop\n\r");
            }
            else if (new_state.gap_scan_state == GAP_SCAN_STATE_SCANNING)
            {
                RPC_DEBUG("[BLE Device] GAP scan start\n\r");
            }
        }
    }
    // As a Server
    else
    {
        if (ble_gap_dev_state.gap_adv_state != new_state.gap_adv_state)
        {
            if (new_state.gap_adv_state == GAP_ADV_STATE_IDLE)
            {
                if (new_state.gap_adv_sub_state == GAP_ADV_TO_IDLE_CAUSE_CONN)
                {
                    RPC_DEBUG("[BLE Device] GAP adv stopped: because connection created\n\r");
                }
                else
                {
                    RPC_DEBUG("[BLE Device] GAP adv stopped\n\r");
                }
            }
            else if (new_state.gap_adv_state == GAP_ADV_STATE_ADVERTISING)
            {
                RPC_DEBUG("[BLE Device] GAP adv start\n\r");
            }
        }
    }

    ble_gap_dev_state = new_state;
}

/**
 * @brief    Handle msg GAP_MSG_LE_CONN_PARAM_UPDATE
 * @note     All the connection parameter update change  events are pre-handled in this function.
 * @param[in] conn_id Connection ID
 * @param[in] status  New update state
 * @param[in] cause Use this cause when status is GAP_CONN_PARAM_UPDATE_STATUS_FAIL
 * @return   void
 */
void ble_param_update_evt_handler(uint8_t conn_id, uint8_t status, uint16_t cause)
{
    switch (status)
    {
    case GAP_CONN_PARAM_UPDATE_STATUS_SUCCESS:
    {
        uint16_t conn_interval;
        uint16_t conn_slave_latency;
        uint16_t conn_supervision_timeout;

        le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, conn_id);
        le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_slave_latency, conn_id);
        le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, conn_id);
        RPC_DEBUG("connParamUpdateEvtHandlerDefault update success:conn_interval 0x%x, conn_slave_latency 0x%x, conn_supervision_timeout 0x%x\n\r", conn_interval, conn_slave_latency, conn_supervision_timeout);
        break;
    }
    case GAP_CONN_PARAM_UPDATE_STATUS_FAIL:
    {
        RPC_DEBUG("connParamUpdateEvtHandlerDefault update failed: cause 0x%x\n\r", cause);
        break;
    }
    case GAP_CONN_PARAM_UPDATE_STATUS_PENDING:
    {
        RPC_DEBUG("connParamUpdateEvtHandlerDefault update pending: conn_id %d\n\r", conn_id);
        break;
    }
    default:
        break;
    }
}

/**
 * @brief    Handle msg GAP_MSG_LE_CONN_MTU_INFO
 * @note     This msg is used to inform APP that exchange mtu procedure is completed.
 * @param[in] conn_id Connection ID
 * @param[in] mtu_size  New mtu size
 * @return   void
 */
void ble_mtu_info_evt_handler(uint8_t conn_id, uint16_t mtu_size)
{
    if (BLEDevice::getClient()->getConnId() == conn_id)
    {
        BLEDevice::getClient()->setMTU(mtu_size);
    }
    RPC_DEBUG("app_handle_conn_mtu_info_evt: conn_id %d, mtu_size %d\n\r", conn_id, mtu_size);
}

/**
 * @brief    Handle msg GAP_MSG_LE_AUTHEN_STATE_CHANGE
 * @note     All the gap authentication state events are pre-handled in this function.
 *           Then the event handling function shall be called according to the new_state
 * @param[in] conn_id Connection ID
 * @param[in] new_state  New authentication state
 * @param[in] cause Use this cause when new_state is GAP_AUTHEN_STATE_COMPLETE
 * @return   void
 */
void ble_authen_state_evt_handler(uint8_t conn_id, uint8_t new_state, uint16_t cause)
{
    RPC_DEBUG("app_handle_authen_state_evt:conn_id %d, cause 0x%x\n\r", conn_id, cause);

    switch (new_state)
    {
    case GAP_AUTHEN_STATE_STARTED:
    {
        RPC_DEBUG("app_handle_authen_state_evt: GAP_AUTHEN_STATE_STARTED\n\r");
    }
    break;

    case GAP_AUTHEN_STATE_COMPLETE:
    {
        if (cause == GAP_SUCCESS)
        {
            RPC_DEBUG("Pair success\n\r");
            RPC_DEBUG("app_handle_authen_state_evt: GAP_AUTHEN_STATE_COMPLETE pair success\n\r");
        }
        else
        {
            RPC_DEBUG("Pair failed: cause 0x%x\n\r", cause);
            RPC_DEBUG("app_handle_authen_state_evt: GAP_AUTHEN_STATE_COMPLETE pair failed\n\r");
        }
    }
    break;

    default:
    {
        RPC_DEBUG("app_handle_authen_state_evt: unknown newstate %d\n\r", new_state);
    }
    break;
    }
}