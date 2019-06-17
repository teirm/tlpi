# Notes on Processes

* If the birth parent of a process dies it is adopted by 'init'.

## Memory Layout of a Process

* text segment -- contains machine instructions.  Read Only.  
  Shareable so that many copies can map in the same segment.

* initialized data segment -- global and static variables that
  are explicitly initialized.

* unitialized data segment -- globals and statics that are not
  explicitly initialized.  BSS section.  Initialized to zero 
  at run time.  Not kept on disk.

* stack -- dynamically growing / shrinking segment for function
  calls

* heap -- dynamically created variables.  Program break.

* Most unix implementations give three globals: etext, edata, and
  end -- pointing to the byte past the end of text, initialized data,
  and unitialized data segments.
