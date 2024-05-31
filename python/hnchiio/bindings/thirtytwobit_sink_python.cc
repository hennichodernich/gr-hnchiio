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
/* BINDTOOL_HEADER_FILE(thirtytwobit_sink.h) */
/* BINDTOOL_HEADER_FILE_HASH(8ac3f976465b90fbcbd5c25a366db60a) */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/hnchiio/thirtytwobit_sink.h>
// pydoc.h is automatically generated in the build directory
#include <thirtytwobit_sink_pydoc.h>

void bind_thirtytwobit_sink(py::module &m) {

  using thirtytwobit_sink = gr::hnchiio::thirtytwobit_sink;

  py::class_<thirtytwobit_sink, gr::block, gr::basic_block,
             std::shared_ptr<thirtytwobit_sink>>(m, "thirtytwobit_sink",
                                                 D(thirtytwobit_sink))

      .def(py::init(&thirtytwobit_sink::make), py::arg("uri"),
           py::arg("device"), py::arg("channels"), py::arg("device_phy"),
           py::arg("params"), py::arg("buffer_size") = DEFAULT_BUFFER_SIZE,
           py::arg("interpolation") = 0, py::arg("cyclic") = false,
           D(thirtytwobit_sink, make))

      ;
}
