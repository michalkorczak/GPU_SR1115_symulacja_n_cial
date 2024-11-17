#include "BHTreeNode.h"
#include <cmath>
#include <algorithm>
#include <iostream>

const double G = 6.67430e-11;
const double THETA = 0.8;

// konstruktor klasy
BHTreeNode::BHTreeNode(const Octant& region_)
    : region(region_), mass(0), centerX(0), centerY(0), centerZ(0) {}

// wstawianie cia³a do drzewa oktantowego
void BHTreeNode::insert(const Body& newBody) {
    
    if (!body) {                                        // jeœli wêze³ nie zawiera ¿adnego cia³a
        if (mass == 0) {                                // jeœli jest pusty
            
            body = std::make_unique<Body>(newBody);     // przypisuje nowe cia³o do wêz³a
            
            // ustawia masê i pozycjê œrodka masy
            mass = newBody.mass;
            centerX = newBody.x;
            centerY = newBody.y;
            centerZ = newBody.z;
        }
        else {                                          // jeœli wêze³ ma ju¿ masê
            if (!children[0])                           // ale nie posiada dzieci (jeszcze nie jest podzielony)
                subdivide();                            // dzieli wêze³ na 8 oktantów (podregiony)
            placeInChild(newBody);                      // wstawia nowe cia³o do odpowiedniego dziecka w podzielonym regionie
        }
    }
    else {                                               // gdy wêze³ zawiera ju¿ cia³o (kolizja)
        // Przenoszenie istniej¹ce cia³a do jednego z podregionów i dodanie nowe cia³a
        Body tempBody = *body;
        body.reset();
        placeInChild(tempBody);
        placeInChild(newBody);
    }

    // aktualizuje masê i pozycjê œrodka masy po dodaniu nowego cia³a
    updateMassAndCenter(newBody);
}

// oblicza si³ê dzia³aj¹c¹ na dane cia³o
void BHTreeNode::calculateForce(const Body& target, double& fx, double& fy, double& fz) const {
    // pomija wêz³y bez masy lub wêze³ bêd¹cy identyczny z `target`
    if (mass == 0.0 || (body && body.get() == &target)) return;

    // oblicza wektor ró¿nicy miêdzy œrodkiem masy regionu a cia³em
    double dx = centerX - target.x;
    double dy = centerY - target.y;
    double dz = centerZ - target.z;
    double dist_sq = dx * dx + dy * dy + dz * dz;           // Odleg³oœæ kwadratowa miêdzy œrodkiem masy a celem
    double dist = sqrt(dist_sq + 1e-10);                    // Odleg³oœæ z ma³ym przesuniêciem, aby unikn¹æ dzielenia przez zero

    // Warunek Barnes-Hut
    if ((region.size / dist) < THETA) {                     // traktuje ca³y region jako punkt
        
        double force = G * mass * target.mass / dist_sq;    // oblicza wartoœæ si³y grawitacyjnej
        // sk³adowe si³y
        fx += force * dx / dist;
        fy += force * dy / dist;
        fz += force * dz / dist;

        // std::cout << "Obliczono sile dla ciala: fx=" << fx << " fy=" << fy << " fz=" << fz << "\n";
    }
    else {
        // jeœli region jest zbyt blisko (przybli¿enie Barnes-Hut nie jest mo¿liwe)
        for (const auto& child : children) {                // Rekurencyjnie oblicza si³y od ka¿dego dziecka
            if (child) child->calculateForce(target, fx, fy, fz);
        }
    }

}

// aktualizuje ca³kowit¹ masê i œrodek masy
void BHTreeNode::updateMassAndCenter(const Body& newBody) {
    double totalMass = mass + newBody.mass;
    centerX = (centerX * mass + newBody.x * newBody.mass) / totalMass;
    centerY = (centerY * mass + newBody.y * newBody.mass) / totalMass;
    centerZ = (centerZ * mass + newBody.z * newBody.mass) / totalMass;
    mass = totalMass;
}

// dzieli wêze³ na 8 podregionów
void BHTreeNode::subdivide() {
    for (int i = 0; i < 8; ++i) {
        children[i] = std::make_unique<BHTreeNode>(region.getSubOctant(i));
    }
}

// przypisuje cia³o do odpowiedniego potomka (dziecka) w drzewie oktantowym
void BHTreeNode::placeInChild(const Body& b) {
    for (int i = 0; i < 8; ++i) {
        if (children[i] && children[i]->region.contains(b)) {
            children[i]->insert(b);
            return;
        }
    }
}

