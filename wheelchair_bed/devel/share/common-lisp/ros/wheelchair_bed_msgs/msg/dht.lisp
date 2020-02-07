; Auto-generated. Do not edit!


(cl:in-package wheelchair_bed_msgs-msg)


;//! \htmlinclude dht.msg.html

(cl:defclass <dht> (roslisp-msg-protocol:ros-message)
  ((temp
    :reader temp
    :initarg :temp
    :type cl:float
    :initform 0.0)
   (humi
    :reader humi
    :initarg :humi
    :type cl:float
    :initform 0.0))
)

(cl:defclass dht (<dht>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <dht>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'dht)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name wheelchair_bed_msgs-msg:<dht> is deprecated: use wheelchair_bed_msgs-msg:dht instead.")))

(cl:ensure-generic-function 'temp-val :lambda-list '(m))
(cl:defmethod temp-val ((m <dht>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader wheelchair_bed_msgs-msg:temp-val is deprecated.  Use wheelchair_bed_msgs-msg:temp instead.")
  (temp m))

(cl:ensure-generic-function 'humi-val :lambda-list '(m))
(cl:defmethod humi-val ((m <dht>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader wheelchair_bed_msgs-msg:humi-val is deprecated.  Use wheelchair_bed_msgs-msg:humi instead.")
  (humi m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <dht>) ostream)
  "Serializes a message object of type '<dht>"
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'temp))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'humi))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <dht>) istream)
  "Deserializes a message object of type '<dht>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'temp) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'humi) (roslisp-utils:decode-single-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<dht>)))
  "Returns string type for a message object of type '<dht>"
  "wheelchair_bed_msgs/dht")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'dht)))
  "Returns string type for a message object of type 'dht"
  "wheelchair_bed_msgs/dht")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<dht>)))
  "Returns md5sum for a message object of type '<dht>"
  "589c35b88a26ab074d5f9b9dc60c010e")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'dht)))
  "Returns md5sum for a message object of type 'dht"
  "589c35b88a26ab074d5f9b9dc60c010e")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<dht>)))
  "Returns full string definition for message of type '<dht>"
  (cl:format cl:nil "float32 temp~%float32 humi~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'dht)))
  "Returns full string definition for message of type 'dht"
  (cl:format cl:nil "float32 temp~%float32 humi~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <dht>))
  (cl:+ 0
     4
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <dht>))
  "Converts a ROS message object to a list"
  (cl:list 'dht
    (cl:cons ':temp (temp msg))
    (cl:cons ':humi (humi msg))
))
