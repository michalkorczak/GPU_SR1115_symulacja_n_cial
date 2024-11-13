#include "BHTreeNode.h"
#include <cmath>
#include <algorithm>
#include <iostream>

const double G = 6.67430e-11;
const double THETA = 0.8;

// konstruktor klasy
BHTreeNode::BHTreeNode(const Octant& region_)
    : region(region_), mass(0), centerX(0), centerY(0), centerZ(0) {}

// wstawianie cia�a do drzewa oktantowego
void BHTreeNode::insert(const Body& newBody) {
    
    if (!body) {                                        // je�li w�ze� nie zawiera �adnego cia�a
        if (mass == 0) {                                // je�li jest pusty
            
            body = std::make_unique<Body>(newBody);     // przypisuje nowe cia�o do w�z�a
            
            // ustawia mas� i pozycj� �rodka masy
            mass = newBody.mass;
            centerX = newBody.x;
            centerY = newBody.y;
            centerZ = newBody.z;
        }
        else {                                          // je�li w�ze� ma ju� mas�
            if (!children[0])                           // ale nie posiada dzieci (jeszcze nie jest podzielony)
                subdivide();                            // dzieli w�ze� na 8 oktant�w (podregiony)
            placeInChild(newBody);                      // wstawia nowe cia�o do odpowiedniego dziecka w podzielonym regionie
        }
    }
    else {                                               // gdy w�ze� zawiera ju� cia�o (kolizja)
        // Przenoszenie istniej�ce cia�a do jednego z podregion�w i dodanie nowe cia�a
        Body tempBody = *body;
        body.reset();
        placeInChild(tempBody);
        placeInChild(newBody);
    }

    // aktualizuje mas� i pozycj� �rodka masy po dodaniu nowego cia�a
    updateMassAndCenter(newBody);
}

// oblicza si�� dzia�aj�c� na dane cia�o
void BHTreeNode::calculateForce(const Body& target, double& fx, double& fy, double& fz) const {
    // pomija w�z�y bez masy lub w�ze� b�d�cy identyczny z `target`
    if (mass == 0.0 || (body && body.get() == &target)) return;

    // oblicza wektor r�nicy mi�dzy �rodkiem masy regionu a cia�em
    double dx = centerX - target.x;
    double dy = centerY - target.y;
    double dz = centerZ - target.z;
    double dist_sq = dx * dx + dy * dy + dz * dz;           // Odleg�o�� kwadratowa mi�dzy �rodkiem masy a celem
    double dist = sqrt(dist_sq + 1e-10);                    // Odleg�o�� z ma�ym przesuni�ciem, aby unikn�� dzielenia przez zero

    // Warunek Barnes-Hut
    if ((region.size / dist) < THETA) {                     // traktuje ca�y region jako punkt
        
        double force = G * mass * target.mass / dist_sq;    // oblicza warto�� si�y grawitacyjnej
        // sk�adowe si�y
        fx += force * dx / dist;
        fy += force * dy / dist;
        fz += force * dz / dist;

        // std::cout << "Obliczono sile dla ciala: fx=" << fx << " fy=" << fy << " fz=" << fz << "\n";
    }
    else {
        // je�li region jest zbyt blisko (przybli�enie Barnes-Hut nie jest mo�liwe)
        for (const auto& child : children) {                // Rekurencyjnie oblicza si�y od ka�dego dziecka
            if (child) child->calculateForce(target, fx, fy, fz);
        }
    }

}

// aktualizuje ca�kowit� mas� i �rodek masy
void BHTreeNode::updateMassAndCenter(const Body& newBody) {
    double totalMass = mass + newBody.mass;
    centerX = (centerX * mass + newBody.x * newBody.mass) / totalMass;
    centerY = (centerY * mass + newBody.y * newBody.mass) / totalMass;
    centerZ = (centerZ * mass + newBody.z * newBody.mass) / totalMass;
    mass = totalMass;
}

// dzieli w�ze� na 8 podregion�w
void BHTreeNode::subdivide() {
    for (int i = 0; i < 8; ++i) {
        children[i] = std::make_unique<BHTreeNode>(region.getSubOctant(i));
    }
}

// przypisuje cia�o do odpowiedniego potomka (dziecka) w drzewie oktantowym
void BHTreeNode::placeInChild(const Body& b) {
    for (int i = 0; i < 8; ++i) {
        if (children[i] && children[i]->region.contains(b)) {
            children[i]->insert(b);
            return;
        }
    }
}

