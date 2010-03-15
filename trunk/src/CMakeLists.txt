FOREACH( mylibfolder 
        osgAudio
        osgPlugin 
    )

    ADD_SUBDIRECTORY(${mylibfolder})

ENDFOREACH( mylibfolder )

# OpenAL/OpenAL++ audio subsystem support
IF(0_ENABLE_SUBSYSTEM_OPENAL)
    ADD_SUBDIRECTORY(openalpp)
ENDIF(0_ENABLE_SUBSYSTEM_OPENAL)
