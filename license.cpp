#include <string>
#include <iostream>
void print_license()
{
	std::string license = R"(
  CSVQuery - Freeware License (Redistributable, Closed Source)
  Copyright (c) 2025 Kwame Yeboah-Gyan. All rights reserved.
  
  < yeboahgyan@gmail.com >
  
  
  This software and its accompanying files (the "Software") are provided free of charge.
  
  By using or distributing the Software, you agree to the following terms:
  
  1. License Grant
  
  You are granted a non-exclusive, worldwide, royalty-free license to use and redistribute the Software in its original, unmodified form.
  No fees may be charged for such redistribution except to cover reasonable media or distribution costs.
  
  2. Restrictions
  
  You may not:
  
  Modify, decompile, disassemble, or reverse engineer the Software.
  
  Remove or alter any copyright or trademark notices.
  
  Distribute modified versions or derivative works of the Software.
  
  Use any part of the Software's code, structure, or design for other projects without permission.
  
  3. Ownership
  
  The Software remains the exclusive property of Kwame Yeboah-Gyan.
  This license does not grant you any ownership rights in the Software.
  
  4. Disclaimer of Warranty
  
  The Software is provided "as is" without warranty of any kind, express or implied, including but not limited to the warranties of merchantability or fitness for a particular purpose.
  
  5. Limitation of Liability
  
  In no event shall the authors or copyright holders be liable for any damages arising from the use, distribution, or inability to use the Software.
  
  6. Termination
  
  This license terminates automatically if you fail to comply with its terms. Upon termination, you must cease using and distributing the Software. )";
	std::cout << license << "\n\n";
}