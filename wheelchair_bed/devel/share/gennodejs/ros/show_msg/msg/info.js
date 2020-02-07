// Auto-generated. Do not edit!

// (in-package show_msg.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;

//-----------------------------------------------------------

class info {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.state = null;
      this.speed = null;
      this.steer_angle = null;
      this.error1 = null;
      this.error2 = null;
    }
    else {
      if (initObj.hasOwnProperty('state')) {
        this.state = initObj.state
      }
      else {
        this.state = '';
      }
      if (initObj.hasOwnProperty('speed')) {
        this.speed = initObj.speed
      }
      else {
        this.speed = 0.0;
      }
      if (initObj.hasOwnProperty('steer_angle')) {
        this.steer_angle = initObj.steer_angle
      }
      else {
        this.steer_angle = 0.0;
      }
      if (initObj.hasOwnProperty('error1')) {
        this.error1 = initObj.error1
      }
      else {
        this.error1 = 0.0;
      }
      if (initObj.hasOwnProperty('error2')) {
        this.error2 = initObj.error2
      }
      else {
        this.error2 = 0.0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type info
    // Serialize message field [state]
    bufferOffset = _serializer.string(obj.state, buffer, bufferOffset);
    // Serialize message field [speed]
    bufferOffset = _serializer.float32(obj.speed, buffer, bufferOffset);
    // Serialize message field [steer_angle]
    bufferOffset = _serializer.float32(obj.steer_angle, buffer, bufferOffset);
    // Serialize message field [error1]
    bufferOffset = _serializer.float32(obj.error1, buffer, bufferOffset);
    // Serialize message field [error2]
    bufferOffset = _serializer.float64(obj.error2, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type info
    let len;
    let data = new info(null);
    // Deserialize message field [state]
    data.state = _deserializer.string(buffer, bufferOffset);
    // Deserialize message field [speed]
    data.speed = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [steer_angle]
    data.steer_angle = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [error1]
    data.error1 = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [error2]
    data.error2 = _deserializer.float64(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += object.state.length;
    return length + 24;
  }

  static datatype() {
    // Returns string type for a message object
    return 'show_msg/info';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '196e07550a586fd28bbab6990451da42';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    string state
    float32 speed
    float32 steer_angle
    float32 error1   
    float64 error2   
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new info(null);
    if (msg.state !== undefined) {
      resolved.state = msg.state;
    }
    else {
      resolved.state = ''
    }

    if (msg.speed !== undefined) {
      resolved.speed = msg.speed;
    }
    else {
      resolved.speed = 0.0
    }

    if (msg.steer_angle !== undefined) {
      resolved.steer_angle = msg.steer_angle;
    }
    else {
      resolved.steer_angle = 0.0
    }

    if (msg.error1 !== undefined) {
      resolved.error1 = msg.error1;
    }
    else {
      resolved.error1 = 0.0
    }

    if (msg.error2 !== undefined) {
      resolved.error2 = msg.error2;
    }
    else {
      resolved.error2 = 0.0
    }

    return resolved;
    }
};

module.exports = info;
