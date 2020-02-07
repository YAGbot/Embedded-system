
(cl:in-package :asdf)

(defsystem "wheelchair_bed_msgs-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "dht" :depends-on ("_package_dht"))
    (:file "_package_dht" :depends-on ("_package"))
  ))