autocmd BufWritePre * :%s/\s\+$//e
syntax enable
set expandtab
set shiftwidth=2
set tabstop=2
set autoindent
set smartindent

set incsearch
set ignorecase

set pastetoggle=<f5>

:nmap j gj
:nmap k gk

set runtimepath^=~/.vim/bundle/ctrlp.vim
execute pathogen#infect()

set background=dark
" solarized options
let g:solarized_termcolors = 256
colorscheme solarized

hi StatusLine term=reverse cterm=reverse ctermbg=239 ctermfg=235
