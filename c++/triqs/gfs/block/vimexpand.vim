" This function finds all occurrences of 
"  VIMEXPAND A B C D
"  WHATEVER BLOCK
"  empty line
"
"  then clean everything after the line containing VIMEXPAND_START
"  and for each VIMEXPAND and for each A, B, C, D, 
"  it copies the pattern at the end of the file and replace @ by A, B, C ...
function! VimExpandSimple()
  " Clean after START  
  normal /VIMEXPAND_STARTjdGo
  normal gg
  while search("VIMEXPAND ", 'W', line("$")) > 0 " limit the search to end of file 
	 
    " Grab the rest of the line in "l and the pattern below in "t (one block)
    normal w"ly$jV}"ty
    let save_cursor = getcurpos()
     for a in split(@l)
      " paste the "t at the end of the file and replace from there till the end @ by a
     execute "normal! G\"tP:.,$s/@/".a."/g"
     endfor  
    call setpos('.', save_cursor)

  endwhile    

  "Close the namespace at the end
  "Clang format
  normal Go}==
endfunction

"----------------------------------------------------------------
"
"Same but arguments A, B, C are now pairs separated by comma (NO SPACE
function! VimExpand2Simple()
  " Clean after START  
  normal /VIMEXPAND_STARTjdGo
  normal gg

  while search("VIMEXPAND2 ", 'W', line("$")) > 0 " limit the search to end of file 
	 
    " Grab the rest of the line in "l and the pattern below in "t (one block)
    normal w"ly$jV}"ty
    let save_cursor = getcurpos()
     for ab in split(@l)
      let [a,b] =  split(ab,',') 
      " paste the "t at the end of the file and replace from there till the end @ by a
      execute "normal! G\"tP:.,$s/@1/".a."/g"
      execute "normal! {:.,$s/@2/".b."/g"
     endfor  
    call setpos('.', save_cursor)

  endwhile    

  "Close the namespace at the end
  "Clang format
  normal Go}==
endfunction 


