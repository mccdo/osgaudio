/* -*-c++-*- */
/**
 * osgAudio - OpenSceneGraph Audio Library
 * (C) Copyright 2009-2012 byKenneth Mark Bryden
 * (programming by Chris 'Xenon' Hanson, AlphaPixel, LLC xenon at alphapixel.com)
 * based on a fork of:
 * Osg AL - OpenSceneGraph Audio Library
 * Copyright (C) 2004 VRlab, Umeå University
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * Please see COPYING file for special static-link exemption to LGPL.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// This file is taken directly from openAL++ Error header

#ifndef OSGAUDIO_ERROR_H
#define OSGAUDIO_ERROR_H 1

#include <iostream>
#include <string>
#include <stdexcept>
#include <osgAudio/Export.h>

namespace osgAudio {

    /**
    * Error class for throwing.
    * The descendants of this class are different error types, and the exact
    * error can be displayed by using "cout << error;" where error is an instance
    * of Error (or one of its descendants)
    */
    class Error : public std::runtime_error {
    public:
        /**
        * Constructor.
        * Will use a default error message.
        */
        Error() : runtime_error("No error description") {}

        /**
        * Constructor.
        * @param description is error message to use.
        */
        Error(const char *description) : runtime_error(description)  {}
        Error(const std::string & description) : runtime_error(description) {}

        /**
        * Copy constructor.
        */
        Error(const Error &error) : runtime_error(error.what())  {}

        /**
        * Function used for printing.
        * @param stream is stream to print to
        * @return the stream with the error message appended.
        */
        std::ostream &put(std::ostream &stream) const;

    protected:
        /**
        * A description of the error
        */
    };

    /**
    * Fatal error.
    * Caused by error in implementation, corrupted memory etc.
    */
    class  FatalError : public Error {
    public:
        /**
        * Constructor.
        * @param description is error message to use.
        */
        FatalError(const char *description) : Error(description) {}
        FatalError(const std::string & description) : Error(description) {}
    };

    /**
    * File error.
    * Caused by wrong file permissions, missing files etc.
    */
    class   FileError : public Error {
    public:
        /**
        * Constructor.
        * @param description is error message to use.
        */
        FileError(const char *description) : Error(description) {}
        FileError(const std::string & description) : Error(description) {}
    };

    /**
    * Memory error.
    * Caused by insufficient memory etc.
    */
    class   MemoryError : public Error {
    public:
        /**
        * Constructor.
        * @param description is error message to use.
        */
        MemoryError(const char *description) : Error(description) {}
        MemoryError(const std::string & description) : Error(description) {}
    };

    /**
    * Name error.
    * Caused by invalid names.
    */
    class   NameError : public Error {
    public:
        /**
        * Constructor.
        * @param description is error message to use.
        */
        NameError(const char *description) : Error(description) {}
        NameError(const std::string & description) : Error(description) {}
    };

    /**
    * Value error.
    * Caused by values out of range etc.
    */
    class   ValueError : public Error {
    public:
        /**
        * Constructor.
        * @param description is error message to use.
        */
        ValueError(const char *description) : Error(description) {}
        ValueError(const std::string & description) : Error(description) {}
    };

    /**
    * Init error.
    * Caused by trying to do actions without proper initialization.
    */
    class   InitError : public Error {
    public:
        /**
        * Constructor.
        * @param description is error message to use.
        */
        InitError(const char *description) : Error(description) {}
        InitError(const std::string & description) : Error(description) {}
    };

    /**
    * Out stream operator.
    * Used to print error messages to a stream (i.e. "cerr << error;").
    * @param stream is the stream to print to.
    * @param error is the error to print.
    * @return the stream.
    */
    OSGAUDIO_EXPORT std::ostream &operator<<(std::ostream &stream,const Error &error);

} // namespace osgAudio

#endif /* OSGAUDIO_ERROR_H */
