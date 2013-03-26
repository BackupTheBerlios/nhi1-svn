"
"  \file       etc/.vimrc
"  \brief      \$Id$
"  
"  (C) 2013 - NHI - #1 - Project - Group
"  
"  \version    \$Rev$
"  \author     EMail: aotto1968 at users.berlios.de
"  \attention  this software has GPL permissions to copy
"              please contact AUTHORS for additional information
"
set background=dark
set softtabstop=2
set shiftwidth=2
set modeline
set mouse=n

syntax on

map <F12> <C-]>
nnoremap <F11> :grep <C-R><C-W> *<CR>:cw<CR>

" Only do this part when compiled with support for autocommands.
if has("autocmd")
  " When editing a file, always jump to the last known cursor position.
  " Don't do it when the position is invalid or when inside an event handler
  " (happens when dropping a file on gvim).
  autocmd BufReadPost *
    \ if line("'\"") > 0 && line("'\"") <= line("$") |
    \   exe "normal g`\"" |
    \ endif

endif " has("autocmd")

