/*
 *  The Simple Earth Project
 *  Copyright (C) 2014 HueSoft LLC
 *  Author: Hector Mendoza, hector.mendoza@huesoftllc.com
 *
 *  This file is part of the Simple Earth Project.
 *
 *  The Simple Earth Project is free software: you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, either version
 *  3 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program. If not, see
 *  <http://www.gnu.org/licenses/>.
 */

#ifndef EXAMPLE_HELLO_WORLD_H
#define EXAMPLE_HELLO_WORLD_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * As it is expected, the Hello World! example is supposed to depict the
 * simplest way to get you started. Therefore this example shows you how to
 * emplace a "hello world" in the world :-). When you run this example, make
 * sure you enable "Labels" from the View menu in order to see the text.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class ExampleHelloWorld
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    ExampleHelloWorld();
    ~ExampleHelloWorld();

    void helloWorld();
};

#endif//EXAMPLE_HELLO_WORLD_H
