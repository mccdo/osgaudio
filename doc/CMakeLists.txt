set( _filesToCopy
    index.html
)

foreach( curFile ${_filesToCopy} )
    configure_file( ${curFile}
        ${PROJECT_BINARY_DIR}/doc/${curFile} COPYONLY )
endforeach()
