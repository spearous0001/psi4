import numpy as np
import psi4
import json

# Generate JSON data
json_data = {
  "schema_name": "QC_JSON",
  "schema_version": "v0.1",
  "molecule": {
    "geometry": [
      0.0,
      0.0,
      -0.1294769411935893,
      0.0,
      -1.494187339479985,
      1.0274465079245698,
      0.0,
      1.494187339479985,
      1.0274465079245698
    ],
    "symbols": [
      "O",
      "H",
      "H"
    ]
  },
  "driver": "gradient",
  "model": {
    "method": "HF",
    "basis": "cc-pVDZ"
  },
  "keywords": {"scf_type": "df"}
}

# Write expected output
expected_return_result = [
  0.0,
  0.0,
  -0.05959774096119619,
  0.0,
  -0.043039786289375104,
  0.02979887048056895,
  0.0,
  0.043039786289375104,
  0.02979887048056895
]
expected_properties = {
  "calcinfo_nbasis": 24,
  "calcinfo_nmo": 24,
  "calcinfo_nalpha": 5,
  "calcinfo_nbeta": 5,
  "calcinfo_natom": 3,
  "scf_one_electron_energy": -122.4452968291507,
  "scf_two_electron_energy": 37.62243738251799,
  "nuclear_repulsion_energy": 8.80146206062943,
  "scf_total_energy": -76.02139738600329
}

psi4.json_wrapper.run_json(json_data)

with open("output.json", "w") as ofile:
    json.dump(json_data, ofile, indent=2)

psi4.compare_integers(True, json_data["success"], "JSON Success")                           #TEST
psi4.compare_arrays(expected_return_result, json_data["return_result"], 6, "Return Value")  #TEST

for k in expected_properties.keys():
    psi4.compare_values(expected_properties[k], json_data["properties"][k], 6, k.upper())   #TEST

with open("output.json", "w") as ofile:                                                     #TEST
    json.dump(json_data, ofile, indent=2)                                                   #TEST
