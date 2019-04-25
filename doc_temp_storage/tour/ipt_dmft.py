from pytriqs.gf import *
from pytriqs.plot.mpl_interface import *
from numpy import *
import os

# Parameters
t = 0.5
beta = 40
n_loops = 20
dos_files = []

# Main loop over U
for U in arange(0.0, 4.05, 0.10):

    # Construct the IPT solver and set initial G
    S = IPTSolver(U = U, beta = beta)
    S.g_iw << SemiCircular(2*t)

    # Do the DMFT loop
    for i in range(n_loops):
        S.g0_iw << inverse( iOmega_n - t**2 * S.g_iw )
        S.solve()

    # Get the real-axis with Pade approximants
    greal = GfReFreq(indices = [1], window = (-4.0,4.0), n_points = 400)
    greal.set_from_pade(S.g_iw, 201, 0.0)

    # Generate the plot
    fig = plt.figure(figsize=(6,6))
    oplot(greal, lw=3, RI='S', figure = fig, label = "DOS")
    plt.xlim(-4,4)
    plt.ylim(0,0.7)
    plt.ylabel("$A(\omega)$")
    plt.title("Local DOS, IPT, Bethe lattice, $\\beta=%.2f$, $U=%.2f$"%(beta,U))

    # Save the plot in a file
    fig.savefig("dos_%s"%U, format="png", transparent=False)
    dos_files.append("dos_%s"%U)
    plt.close(fig)

# Create an animated GIF (you need to have 'convert' installed)
print "Creating an animated dos plot..."
os.system("convert -delay 25 -loop 0 %s dos.gif"%(" ".join(dos_files)))
