/*
 * @BEGIN LICENSE
 *
 * Psi4: an open-source quantum chemistry software package
 *
 * Copyright (c) 2007-2018 The Psi4 Developers.
 *
 * The copyrights for code used from other parties are included in
 * the corresponding files.
 *
 * This file is part of Psi4.
 *
 * Psi4 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * Psi4 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with Psi4; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * @END LICENSE
 */

#include "psi4/pybind11.h"

#include "psi4/libfock/jk.h"
#include "psi4/libfock/soscf.h"
#include "psi4/lib3index/denominator.h"
#include "psi4/lib3index/dftensor.h"
#include "psi4/lib3index/df_helper.h"
#include "psi4/libmints/molecule.h"
#include "psi4/libmints/matrix.h"
#include "psi4/libmints/vector.h"
#include "psi4/libmints/basisset.h"
#include "psi4/libmints/wavefunction.h"
#include "psi4/libpsi4util/process.h"
#include "psi4/libscf_solver/sad.h"

using namespace psi;

void export_fock(py::module &m) {
    py::class_<JK, std::shared_ptr<JK>>(m, "JK", "docstring")
        .def_static("build_JK",
                    [](std::shared_ptr<BasisSet> basis, std::shared_ptr<BasisSet> aux) {
                        return JK::build_JK(basis, aux, Process::environment.options);
                    })
        .def("initialize", &JK::initialize)
        .def("set_cutoff", &JK::set_cutoff)
        .def("set_memory", &JK::set_memory)
        .def("set_omp_nthread", &JK::set_omp_nthread)
        .def("set_do_J", &JK::set_do_J)
        .def("set_do_K", &JK::set_do_K)
        .def("set_do_wK", &JK::set_do_wK)
        .def("set_omega", &JK::set_omega)
        .def("compute", &JK::compute)
        .def("finalize", &JK::finalize)
        .def("C_clear",
             [](JK &jk) {
                 jk.C_left().clear();
                 jk.C_right().clear();
             })
        .def("C_add",
             [](JK &jk, SharedMatrix Cl) {
                 jk.C_left().push_back(Cl);
                 jk.C_right().push_back(Cl);
             })
        .def("C_left_add", [](JK &jk, SharedMatrix Cl) { jk.C_left().push_back(Cl); })
        .def("C_right_add", [](JK &jk, SharedMatrix Cr) { jk.C_right().push_back(Cr); })
        .def("J", &JK::J, py::return_value_policy::reference_internal)
        .def("K", &JK::K, py::return_value_policy::reference_internal)
        .def("wK", &JK::wK, py::return_value_policy::reference_internal)
        .def("D", &JK::D, py::return_value_policy::reference_internal)
        .def("print_header", &JK::print_header, "docstring");

    py::class_<LaplaceDenominator, std::shared_ptr<LaplaceDenominator>>(m, "LaplaceDenominator",
                                                                        "docstring")
        .def(py::init<std::shared_ptr<Vector>, std::shared_ptr<Vector>, double>())
        .def("denominator_occ", &LaplaceDenominator::denominator_occ, "docstring")
        .def("denominator_vir", &LaplaceDenominator::denominator_vir, "docstring");

    py::class_<DFTensor, std::shared_ptr<DFTensor>>(m, "DFTensor", "docstring")
        .def(py::init<std::shared_ptr<BasisSet>, std::shared_ptr<BasisSet>, std::shared_ptr<Matrix>,
                      int, int>())
        .def("Qso", &DFTensor::Qso, "doctsring")
        .def("Qmo", &DFTensor::Qmo, "doctsring")
        .def("Qoo", &DFTensor::Qoo, "doctsring")
        .def("Qov", &DFTensor::Qov, "doctsring")
        .def("Qvv", &DFTensor::Qvv, "doctsring")
        .def("Imo", &DFTensor::Imo, "doctsring")
        .def("Idfmo", &DFTensor::Idfmo, "doctsring");

    py::class_<FittingMetric, std::shared_ptr<FittingMetric>>(m, "FittingMetric", "docstring")
        .def(py::init<std::shared_ptr<BasisSet>, bool>())
        .def("get_algorithm", &FittingMetric::get_algorithm, "docstring")
        .def("is_poisson", &FittingMetric::is_poisson, "docstring")
        .def("is_inverted", &FittingMetric::is_inverted, "docstring")
        .def("get_metric", &FittingMetric::get_metric, "docstring")
        .def("get_pivots", &FittingMetric::get_pivots, "docstring")
        .def("get_reverse_pivots", &FittingMetric::get_reverse_pivots, "docstring")
        .def("form_fitting_metric", &FittingMetric::form_fitting_metric, "docstring")
        .def("form_cholesky_inverse", &FittingMetric::form_cholesky_inverse, "docstring")
        .def("form_QR_inverse", &FittingMetric::form_QR_inverse, "docstring")
        .def("form_eig_inverse", &FittingMetric::form_eig_inverse, "docstring")
        .def("form_full_inverse", &FittingMetric::form_full_inverse, "docstring");

    py::class_<SOMCSCF, std::shared_ptr<SOMCSCF>>(m, "SOMCSCF", "docstring")
        // .def(init<std::shared_ptr<JK>, SharedMatrix, SharedMatrix >())
        .def("Ck", &SOMCSCF::Ck)
        .def("form_rotation_matrix", &SOMCSCF::form_rotation_matrix, py::arg("x"),
             py::arg("order") = 2)
        .def("rhf_energy", &SOMCSCF::rhf_energy)
        .def("update", &SOMCSCF::update)
        .def("approx_solve", &SOMCSCF::approx_solve)
        .def("solve", &SOMCSCF::solve)
        .def("H_approx_diag", &SOMCSCF::H_approx_diag)
        .def("compute_Hk", &SOMCSCF::Hk)
        .def("compute_Q", &SOMCSCF::compute_Q)
        .def("compute_Qk", &SOMCSCF::compute_Qk)
        .def("compute_AFock", &SOMCSCF::compute_AFock)
        .def("current_total_energy", &SOMCSCF::current_total_energy)
        .def("current_docc_energy", &SOMCSCF::current_docc_energy)
        .def("current_ci_energy", &SOMCSCF::current_ci_energy)
        .def("current_AFock", &SOMCSCF::current_AFock)
        .def("current_IFock", &SOMCSCF::current_IFock)
        .def("zero_redundant", &SOMCSCF::zero_redundant)
        .def("gradient", &SOMCSCF::gradient)
        .def("gradient_rms", &SOMCSCF::gradient_rms);

    py::class_<DFSOMCSCF, std::shared_ptr<DFSOMCSCF>, SOMCSCF>(m, "DFSOMCSCF", "docstring");
    py::class_<DiskSOMCSCF, std::shared_ptr<DiskSOMCSCF>, SOMCSCF>(m, "DiskSOMCSCF", "docstring");

    // DF Helper
    typedef SharedMatrix (DF_Helper::*take_string)(std::string);
    typedef SharedMatrix (DF_Helper::*tensor_access3)(
        std::string, std::vector<size_t>, std::vector<size_t>, std::vector<size_t>);

    py::class_<DF_Helper, std::shared_ptr<DF_Helper>>(m, "DF_Helper",
                                                                            "docstring")
        .def(py::init<std::shared_ptr<BasisSet>, std::shared_ptr<BasisSet>>())
        .def("set_memory", &DF_Helper::set_memory)
        .def("get_memory", &DF_Helper::get_memory)
        .def("set_method", &DF_Helper::set_method)
        .def("get_method", &DF_Helper::get_method)
        .def("get_AO_size", &DF_Helper::get_AO_size)
        .def("set_nthreads", &DF_Helper::set_nthreads)
        .def("hold_met", &DF_Helper::hold_met)
        .def("set_schwarz_cutoff", &DF_Helper::set_schwarz_cutoff)
        .def("get_schwarz_cutoff", &DF_Helper::get_schwarz_cutoff)
        .def("set_AO_core", &DF_Helper::set_AO_core)
        .def("get_AO_core", &DF_Helper::get_AO_core)
        .def("set_MO_core", &DF_Helper::set_MO_core)
        .def("get_MO_core", &DF_Helper::get_MO_core)
        .def("add_space", &DF_Helper::add_space)
        .def("initialize", &DF_Helper::initialize)
        .def("print_header", &DF_Helper::print_header)
        .def("add_transformation", &DF_Helper::add_transformation, py::arg("name"),
             py::arg("key1"), py::arg("key2"), py::arg("order") = "Qpq")
        .def("transform", &DF_Helper::transform)
        .def("clear_spaces", &DF_Helper::clear_spaces)
        .def("clear_all", &DF_Helper::clear_all)
        .def("transpose", &DF_Helper::transpose)
        .def("get_space_size", &DF_Helper::get_space_size)
        .def("get_tensor_size", &DF_Helper::get_tensor_size)
        .def("get_tensor_shape", &DF_Helper::get_tensor_shape)
        .def("get_tensor", take_string(&DF_Helper::get_tensor))
        .def("get_tensor", tensor_access3(&DF_Helper::get_tensor))
        .def("set_JK_hint", &DF_Helper::set_JK_hint)
        .def("build_JK", &DF_Helper::build_JK);

    py::class_<scf::SADGuess, std::shared_ptr<scf::SADGuess>>(m, "SADGuess", "docstring")
        .def_static("build_SAD",
        [](std::shared_ptr<BasisSet> basis, std::vector<std::shared_ptr<BasisSet>> atomic_bases, int i, int j) 
        { 
           return scf::SADGuess(basis, atomic_bases, i, j, Process::environment.options);
        })
        .def("compute_guess", &scf::SADGuess::compute_guess)
        .def("set_print", &scf::SADGuess::set_print)
        .def("set_debug", &scf::SADGuess::set_debug)    
        .def("set_atomic_fit_bases", &scf::SADGuess::set_atomic_fit_bases)
        .def("Da", &scf::SADGuess::Da)
        .def("Db", &scf::SADGuess::Db)
        .def("Ca", &scf::SADGuess::Ca)
        .def("Cb", &scf::SADGuess::Cb);
}
