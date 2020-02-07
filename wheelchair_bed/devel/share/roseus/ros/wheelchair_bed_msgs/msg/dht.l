;; Auto-generated. Do not edit!


(when (boundp 'wheelchair_bed_msgs::dht)
  (if (not (find-package "WHEELCHAIR_BED_MSGS"))
    (make-package "WHEELCHAIR_BED_MSGS"))
  (shadow 'dht (find-package "WHEELCHAIR_BED_MSGS")))
(unless (find-package "WHEELCHAIR_BED_MSGS::DHT")
  (make-package "WHEELCHAIR_BED_MSGS::DHT"))

(in-package "ROS")
;;//! \htmlinclude dht.msg.html


(defclass wheelchair_bed_msgs::dht
  :super ros::object
  :slots (_temp _humi ))

(defmethod wheelchair_bed_msgs::dht
  (:init
   (&key
    ((:temp __temp) 0.0)
    ((:humi __humi) 0.0)
    )
   (send-super :init)
   (setq _temp (float __temp))
   (setq _humi (float __humi))
   self)
  (:temp
   (&optional __temp)
   (if __temp (setq _temp __temp)) _temp)
  (:humi
   (&optional __humi)
   (if __humi (setq _humi __humi)) _humi)
  (:serialization-length
   ()
   (+
    ;; float32 _temp
    4
    ;; float32 _humi
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _temp
       (sys::poke _temp (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _humi
       (sys::poke _humi (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _temp
     (setq _temp (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _humi
     (setq _humi (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get wheelchair_bed_msgs::dht :md5sum-) "589c35b88a26ab074d5f9b9dc60c010e")
(setf (get wheelchair_bed_msgs::dht :datatype-) "wheelchair_bed_msgs/dht")
(setf (get wheelchair_bed_msgs::dht :definition-)
      "float32 temp
float32 humi

")



(provide :wheelchair_bed_msgs/dht "589c35b88a26ab074d5f9b9dc60c010e")


