; Perform C-c C-e h on documentation.org
; 
; Colouring of source code is not performed in batch mode
; unfortunately.

(require 'org)
(add-to-list 'load-path default-directory)
(find-file "documentation.org")
(org-export-as-html-batch)
