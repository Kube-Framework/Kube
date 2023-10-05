/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Debug helper
 */

#pragma once

#ifdef NDEBUG // Release mode
# define KUBE_DEBUG_BUILD false
#else // Debug mode
# define KUBE_DEBUG_BUILD true
#endif
