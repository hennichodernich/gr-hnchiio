/*
 * Copyright 2024 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually
 * edited  */
/* The following lines can be configured to regenerate this file during cmake */
/* If manual edits are made, the following tags should be modified accordingly.
 */
/* BINDTOOL_GEN_AUTOMATIC(0) */
/* BINDTOOL_USE_PYGCCXML(0) */
/* BINDTOOL_HEADER_FILE(hnchbbboard_source.h) */
/* BINDTOOL_HEADER_FILE_HASH(49e3ffb16e2c0e5bc8f3787656fc957e) */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/hnchiio/hnchbbboard_source.h>
// pydoc.h is automatically generated in the build directory
#include <hnchbbboard_source_pydoc.h>

void bind_hnchbbboard_source(py::module &m) {

  using hnchbbboard_source = gr::hnchiio::hnchbbboard_source;

  py::class_<hnchbbboard_source, gr::block, gr::basic_block,
             std::shared_ptr<hnchbbboard_source>>(m, "hnchbbboard_source",
                                                  D(hnchbbboard_source))

      .def(py::init(&hnchbbboard_source::make), py::arg("uri"),
           py::arg("longfrequency"), py::arg("samplerate"),
           py::arg("stream_id"), py::arg("buffer_size"), py::arg("gain_value"),
           D(hnchbbboard_source, make))

      ;
}
