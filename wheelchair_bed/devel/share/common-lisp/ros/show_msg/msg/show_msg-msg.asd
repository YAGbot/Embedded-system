
(cl:in-package :asdf)

(defsystem "show_msg-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "info" :depends-on ("_package_info"))
    (:file "_package_info" :depends-on ("_package"))
  ))