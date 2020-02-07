;; Auto-generated. Do not edit!


(when (boundp 'show_msg::info)
  (if (not (find-package "SHOW_MSG"))
    (make-package "SHOW_MSG"))
  (shadow 'info (find-package "SHOW_MSG")))
(unless (find-package "SHOW_MSG::INFO")
  (make-package "SHOW_MSG::INFO"))

(in-package "ROS")
;;//! \htmlinclude info.msg.html


(defclass show_msg::info
  :super ros::object
  :slots (_state _speed _steer_angle _error1 _error2 ))

(defmethod show_msg::info
  (:init
   (&key
    ((:state __state) "")
    ((:speed __speed) 0.0)
    ((:steer_angle __steer_angle) 0.0)
    ((:error1 __error1) 0.0)
    ((:error2 __error2) 0.0)
    )
   (send-super :init)
   (setq _state (string __state))
   (setq _speed (float __speed))
   (setq _steer_angle (float __steer_angle))
   (setq _error1 (float __error1))
   (setq _error2 (float __error2))
   self)
  (:state
   (&optional __state)
   (if __state (setq _state __state)) _state)
  (:speed
   (&optional __speed)
   (if __speed (setq _speed __speed)) _speed)
  (:steer_angle
   (&optional __steer_angle)
   (if __steer_angle (setq _steer_angle __steer_angle)) _steer_angle)
  (:error1
   (&optional __error1)
   (if __error1 (setq _error1 __error1)) _error1)
  (:error2
   (&optional __error2)
   (if __error2 (setq _error2 __error2)) _error2)
  (:serialization-length
   ()
   (+
    ;; string _state
    4 (length _state)
    ;; float32 _speed
    4
    ;; float32 _steer_angle
    4
    ;; float32 _error1
    4
    ;; float64 _error2
    8
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; string _state
       (write-long (length _state) s) (princ _state s)
     ;; float32 _speed
       (sys::poke _speed (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _steer_angle
       (sys::poke _steer_angle (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _error1
       (sys::poke _error1 (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float64 _error2
       (sys::poke _error2 (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; string _state
     (let (n) (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4) (setq _state (subseq buf ptr- (+ ptr- n))) (incf ptr- n))
   ;; float32 _speed
     (setq _speed (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _steer_angle
     (setq _steer_angle (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _error1
     (setq _error1 (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float64 _error2
     (setq _error2 (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;;
   self)
  )

(setf (get show_msg::info :md5sum-) "196e07550a586fd28bbab6990451da42")
(setf (get show_msg::info :datatype-) "show_msg/info")
(setf (get show_msg::info :definition-)
      "string state
float32 speed
float32 steer_angle
float32 error1   
float64 error2   

")



(provide :show_msg/info "196e07550a586fd28bbab6990451da42")


