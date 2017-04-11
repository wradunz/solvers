/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2016 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    simpleFoam

Description
    Steady-state solver for incompressible, turbulent flow, using the SIMPLE
    algorithm.

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "singlePhaseTransportModel.H"
// Typedefs for turbulence RAS and LES
#include "turbulentTransportModel.H"
#include "simpleControl.H"
#include "fvOptions.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// argc: the number of arguments; *argv[]: the arguments e.g. simpleFoam -case hhasl2D 
int main(int argc, char *argv[])
{
    #include "postProcess.H"

    #include "setRootCase.H"
    // Creates runTime object
    #include "createTime.H"
    // Creates mesh object from the class fvMesh
    #include "createMesh.H"
    // Includes piso, pimple or simple control  -- simpleControl simple(mesh); which CREATES the object simple
    #include "createControl.H"
    // Creates fields p and U and objects turbulence AND laminarTransport -- createFields calls TurbulenceModel.C New, which reads dictionary for RAS or LES, in our case it points to RASModel.C New which reads subdictionary for turbulence model which finally points to the kEpsilon model. 
    #include "createFields.H"
//before this point we already know which turb model is being used
    // IDK WTF
    #include "createFvOptions.H"
    // Declares scalar cumulativeContErr = 0;
    #include "initContinuityErrs.H"
    // Validate the turbulence fields after construction (validate() declared in turbulenceModel.H -- where is it implemented??)
    turbulence->validate();

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nStarting time loop\n" << endl;

    while (simple.loop())
    {
        Info<< "Time = " << runTime.timeName() << nl << endl;

        // --- Pressure-velocity SIMPLE corrector
        {
            #include "UEqn.H"
            #include "pEqn.H"
        }

	// there is a laminarTransport object and a member function -- find where the object is created and the class member function is implemented.

//correct() is declared in singlePhaseTransportModel.H
        laminarTransport.correct();

	// Solve the turbulence equations and correct the turbulence viscosity
        turbulence->correct();

	// Write the fields to time directories
        runTime.write();

        Info<< "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
            << nl << endl;
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
