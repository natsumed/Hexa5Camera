#include "message.h"

std::string SIYI_Message::increment_seq(int val) {
    if (val < 0 || val > 65535) {
        _seq = 0;
        return "0000";
    }

    int seq = val + 1;
    _seq = seq;

    std::stringstream ss;
    ss << std::hex << std::setw(4) << std::setfill('0') << seq;
    std::string seq_hex = ss.str();
    seq_hex = seq_hex.substr(2);

    if (seq_hex.length() == 3) seq_hex = "0" + seq_hex;
    else if (seq_hex.length() == 2)seq_hex = "00" + seq_hex;
    else if (seq_hex.length() == 1) seq_hex = "000" + seq_hex;
    else seq_hex = "0000";

    std::string low_b = seq_hex.substr(seq_hex.length() - 2);
    std::string high_b = seq_hex.substr(0, 2);
    std::string seq_str = low_b + high_b;

    return seq_str;
}

std::string SIYI_Message::compute_data_len(std::string data) {
    if (data.length() % 2 != 0) data = "0" + data;

    int L = int(data.length() / 2);

    std::stringstream ss;
    ss << std::hex << std::setw(4) << std::setfill('0') << L;
    std::string len_hex = ss.str();
    len_hex = len_hex.substr(2);

    if (len_hex.length() == 3) len_hex = "0" + len_hex;
    else if (len_hex.length() == 2) len_hex = "00" + len_hex;
    else if (len_hex.length() == 1) len_hex = "000" + len_hex;
    else len_hex = "0000";

    std::string low_b = len_hex.substr(len_hex.length() - 2);
    std::string high_b = len_hex.substr(0, 2);
    std::string len_str = low_b + high_b;

    return len_str;
}

std::tuple<std::string, int, std::string, int> SIYI_Message::decode_msg(const std::string &msg) const {
    std::string data, cmd_id;
    int data_len, seq = 0;

    if (msg.length() < MINIMUM_DATA_LENGTH) {
        std::cout << "Warning, message length is not long enough for decoding" << std::endl;
        return std::make_tuple(data, data_len, cmd_id, seq);
    }

    // Check data length, bytes are reversed, according to SIYI built-in SDK
    std::string low_b = msg.substr(6, 2);
    std::string high_b = msg.substr(8, 2);
    std::string data_len_str = high_b + low_b;
    data_len = std::stoi(data_len_str, nullptr, 16);

    // Perform CRC16 checkout
    std::string msg_crc = msg.substr(msg.length() - 4);
    std::string payload = msg.substr(0, msg.length() - 4);
    std::string crc = CRC16::compute_str_swap(payload);
    if (crc != msg_crc) {
        std::cout << "Warning, CRC16 error during message decoding" << std::endl;
        return std::make_tuple(data, data_len, cmd_id, seq);
    }

    // Get sequence
    low_b = msg.substr(10, 2);
    high_b = msg.substr(12, 2);
    std::string seq_hex = high_b + low_b;
    seq = std::stoi(seq_hex, nullptr, 16);

    // Get command ID
    cmd_id = msg.substr(14, 2);

    // Get data
    if (data_len > 0) data = msg.substr(16, data_len * 2); // *2 because each byte is 2 chars, e.g. "ff"

    return std::make_tuple(data, data_len, cmd_id, seq);
}

#include <cstdint>  // For uint8_t
#include <iomanip>  // For std::setw/std::setfill
std::string SIYI_Message::encode_msg(const std::string &data, const std::string &cmd_id) {
    // Protocol Structure:
    // [Header][CTRL][SEQ][LEN][CMD][DATA][CRC]
    
    const std::string HEADER = "5566";      // 2 bytes
    const std::string CTRL_BYTE = "01";     // 1 byte (command type)
    
    // Sequence (2 bytes, little-endian)
    std::string seq = increment_seq(_seq);  // Ensure returns 4-char hex (e.g., "0001")
    
    // Data length (2 bytes, little-endian)
    int data_bytes = data.length() / 2;     // 2 hex chars = 1 byte
    std::stringstream len_ss;
    len_ss << std::setw(4) << std::setfill('0') << std::hex << data_bytes;
    std::string data_len = len_ss.str().substr(2, 2) + len_ss.str().substr(0, 2);
    
    // Command ID (1 byte)
    if (cmd_id.length() != 2) {  // cmd_id must be 1 byte (2 hex chars)
        std::cerr << "Invalid CMD_ID: " << cmd_id << std::endl;
        return "";
    }
    
    // Build message
    std::string msg = HEADER + CTRL_BYTE + seq + data_len + cmd_id + data;
    
    // Compute CRC16
    std::string crc = CRC16::compute_str_swap(msg);
    if (crc.empty()) return "";
    
    return msg + crc;
}
/////////////////////////
// MESSAGE DEFINITIONS //
/////////////////////////

std::string SIYI_Message::firmware_version_msg() {
    std::string data;
    std::string cmd_id = ACQUIRE_FIRMWARE_VERSION;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::hardware_id_msg() {
    std::string data;
    std::string cmd_id = ACQUIRE_HARDWARE_ID;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::autofocus_msg() {
    std::string data = "01";
    std::string cmd_id = AUTOFOCUS;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::zoom_in_msg() {
    std::string data = "01";
    std::string cmd_id = MANUAL_ZOOM;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::zoom_out_msg() {
    std::string data = "ff";
    std::string cmd_id = MANUAL_ZOOM;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::zoom_halt_msg() {
    std::string data = "00";
    std::string cmd_id = MANUAL_ZOOM;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::absolute_zoom_msg(int integer, int fractional) {
    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0') << integer;
    std::string data1 = ss.str();
    data1 = data1.substr(data1.length() - 2);

    ss.clear();
    ss << std::hex << std::setw(2) << std::setfill('0') << fractional;
    std::string data2 = ss.str();
    data2 = data2.substr(data2.length() - 2);

    std::string data = data1 + data2;
    std::string cmd_id = ABSOLUTE_ZOOM;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::maximum_zoom_msg() {
    std::string data;
    std::string cmd_id = ACQUIRE_MAX_ZOOM;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::focus_far_msg() {
    std::string data = "01";
    std::string cmd_id = MANUAL_FOCUS;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::focus_close_msg() {
    std::string data = "ff";
    std::string cmd_id = MANUAL_FOCUS;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::focus_halt_msg() {
    std::string data = "00";
    std::string cmd_id = MANUAL_FOCUS;
    return SIYI_Message::encode_msg(data, cmd_id);
}

#include <algorithm>  // For std::clamp
std::string SIYI_Message::gimbal_speed_msg(int yaw_speed, int pitch_speed) {
    // Clamp to [-100, 100]
    yaw_speed = (yaw_speed > 100) ? 100 : (yaw_speed < -100) ? -100 : yaw_speed;
    pitch_speed = (pitch_speed > 100) ? 100 : (pitch_speed < -100) ? -100 : pitch_speed;

    // Convert to 16-bit little-endian
    uint16_t yaw_le = static_cast<uint16_t>(static_cast<int16_t>(yaw_speed));
    uint16_t pitch_le = static_cast<uint16_t>(static_cast<int16_t>(pitch_speed));

    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0') << (yaw_le & 0xFF)
       << std::hex << std::setw(2) << std::setfill('0') << ((yaw_le >> 8) & 0xFF)
       << std::hex << std::setw(2) << std::setfill('0') << (pitch_le & 0xFF)
       << std::hex << std::setw(2) << std::setfill('0') << ((pitch_le >> 8) & 0xFF);

    return SIYI_Message::encode_msg(ss.str(), GIMBAL_ROTATION);
}

std::string SIYI_Message::gimbal_center_msg() {
    std::string data = "01";
    std::string cmd_id = CENTER;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::gimbal_info_msg() {
    std::string data;
    std::string cmd_id = ACQUIRE_GIMBAL_INFO;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::lock_mode_msg() {
    std::string data = "03";
    std::string cmd_id = PHOTO_VIDEO;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::follow_mode_msg() {
    std::string data = "04";
    std::string cmd_id = PHOTO_VIDEO;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::fpv_mode_msg() {
    std::string data = "05";
    std::string cmd_id = PHOTO_VIDEO;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::function_feedback_msg() {
    std::string data;
    std::string cmd_id = FUNCTION_FEEDBACK_INFO;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::photo_msg() {
    std::string data = "00";
    std::string cmd_id = PHOTO_VIDEO;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::record_msg() {
    std::string data = "02";
    std::string cmd_id = PHOTO_VIDEO;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::gimbal_attitude_msg() {
    std::string data;
    std::string cmd_id = ACQUIRE_GIMBAL_ATTITUDE;
    return SIYI_Message::encode_msg(data, cmd_id);
}

std::string SIYI_Message::gimbal_angles_msg(float yaw, float pitch) {
    // Check if yaw angle exceeded limit and convert it to hex string
    if (yaw > 135.) yaw = 135.;
    else if (yaw < -135.) yaw = -135.;
    yaw = -yaw;  // reverse yaw as for some reason it is positive when turning clockwise by default
    int control_yaw = static_cast<int>(yaw * 10);
    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0') << control_yaw;
    std::string data1 = ss.str();
    data1 = data1.substr(data1.length() - 2);

    // Check if pitch angle exceeded limit and convert it to hex string
    if (pitch > 25.) pitch = 25.;
    else if (pitch < -90.) pitch = -90.;
    int control_pitch = static_cast<int>(pitch * 10);
    ss.clear();
    ss << std::hex << std::setw(2) << std::setfill('0') << control_pitch;
    std::string data2 = ss.str();
    data2 = data2.substr(data2.length() - 2);

    std::string data = data1 + data2;
    std::string cmd_id = CONTROL_ANGLE;
    return SIYI_Message::encode_msg(data, cmd_id);
}
