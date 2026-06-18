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

    def test_spawn_key_streams(self):
        for name in random_generator_names_list():
            r_a = RandomGenerator(name, 198, spawn_key=[0, 0])
            r_b = RandomGenerator(name, 198, [0, 0])  # positional spawn key
            r_t = RandomGenerator(name, 198, spawn_key=[0, 1])
            r_0 = RandomGenerator(name, 198)
            diff_t, diff_0 = False, False
            for _ in range(100):
                a = r_a()
                self.assertEqual(a, r_b())
                diff_t |= a != r_t()
                diff_0 |= a != r_0()
            self.assertTrue(diff_t)
            self.assertTrue(diff_0)

    def test_buffer_size(self):
        rng1 = RandomGenerator("mt19937_64", 42)
        rng2 = RandomGenerator("mt19937_64", 42, buffer_size=10)
        for _ in range(100):
            self.assertEqual(rng1(), rng2())


if __name__ == '__main__':
    unittest.main()
