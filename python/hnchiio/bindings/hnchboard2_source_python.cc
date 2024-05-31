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
/* BINDTOOL_HEADER_FILE(hnchboard2_source.h) */
/* BINDTOOL_HEADER_FILE_HASH(72742766ac6b09d52401c61de3dbfd02) */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/hnchiio/hnchboard2_source.h>
// pydoc.h is automatically generated in the build directory
#include <hnchboard2_source_pydoc.h>

void bind_hnchboard2_source(py::module &m) {

  using hnchboard2_source = gr::hnchiio::hnchboard2_source;

  py::class_<hnchboard2_source, gr::block, gr::basic_block,
             std::shared_ptr<hnchboard2_source>>(m, "hnchboard2_source",
                                                 D(hnchboard2_source))

      .def(py::init(&hnchboard2_source::make), py::arg("uri"),
           py::arg("samplerate"), py::arg("lo_freq"), py::arg("gain_value"),
           py::arg("buffer_size"), D(hnchboard2_source, make))

      ;
}