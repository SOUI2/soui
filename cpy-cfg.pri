CONFIG(LIB_ALL){
	system(copy "config\options\liball.h" "config\core-def.h")
}
else{
	CONFIG(LIB_SOUI){
		system(copy "config\options\lib.h" "config\core-def.h")
	}
	else{
		system(copy "config\options\dll.h" "config\core-def.h")
	}
}

!LIB_ALL:!COM_LIB{
	system(copy "config\options\com-dll.h" "config\com-def.h")
}
else{
	system(copy "config\options\com-lib.h" "config\com-def.h")
}


