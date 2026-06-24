# Copyright (c) 2026 Simons Foundation
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You may obtain a copy of the License at
#     https:#www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Nils Wentzell

import unittest

from triqs.mc_tools import RandomGenerator, random_generator_names_list


class TestRandomGenerator(unittest.TestCase):

    def test_reproducibility(self):
        for name in random_generator_names_list():
            rng1 = RandomGenerator(name, 12345)
            rng2 = RandomGenerator(name, 12345)
            for _ in range(50):
                self.assertEqual(rng1(), rng2())

if __name__ == '__main__':
    unittest.main()
